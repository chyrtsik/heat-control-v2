#include <Arduino.h>
#include <ArduinoJson.h>
#include <OneWire.h> 
#include <DallasTemperature.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <EspDebug.h>
#include <ParallelBus.h>
#include <BusServo.h>
#include <Switch.h>
#include <ConsulRegistration.h>
#include <FlowSensor.h>
#include <HeatingPowerSensor.h>
#include <TermoRelay.h>
#include <ConditionalRelay.h>
#include <ServoController.h>
#include <DallasTemperatureSensor.h>
#include <HighTemperatureSensor.h>

#include "WiFiCredentials.h"
#include "Settings.h"

#include <Workflow.h>

//////////////////////////////////////////////////
//Temperature sensors addresses (do not forget to disable dev board sensors upon merge)

#ifdef DEBUG_BOARD

const char *SERVICE_NAME = "heating-dev";    //"-dev" indicates development board.
// DeviceAddress boardSensorAddress   = {40, 101, 180, 11, 11, 0, 0, 51};   //SMD DEV board sensor
DeviceAddress boardSensorAddress   = { 40, 255, 55, 222, 35, 23, 3, 153};   //DIP DEV board sensor
DeviceAddress &boilerSensorAddress = boardSensorAddress;
DeviceAddress &feedSensorAddress   = boardSensorAddress;
DeviceAddress &returnSensorAddress  = boardSensorAddress;
DeviceAddress &outsideSensorAddress = boardSensorAddress;
DeviceAddress &insideSensorAddress  = boardSensorAddress;
DeviceAddress &flueSensorAddress  = boardSensorAddress;

#else //DEBUG_BOARD

const char *SERVICE_NAME = "heating";    
DeviceAddress boardSensorAddress   = {40,  63, 193,  11,  11,   0,  0, 109};   //Different for each board
DeviceAddress boilerSensorAddress  = {40,   0,   9,   0, 226,  56, 34, 161};
DeviceAddress feedSensorAddress    = {40,   0,   9,   0,   1, 110, 34,  95};
DeviceAddress returnSensorAddress  = {40,   0,   9,   0, 245,  57, 34,  85};
DeviceAddress outsideSensorAddress = {40, 255,  45, 233,  35,  23,  3, 150};
DeviceAddress insideSensorAddress  = {40, 255, 228,  13,  36,  23,  3, 122};
DeviceAddress flueSensorAddress  =   {40, 255, 232,  17,  36,  23,  3, 113};

#endif

//////////////////////////////////////////////
// Parallel bus
const int bits_no_select[] = {0,0,0,0,0,0,0,0};
const int bits_all_select[] = {1,1,1,1,1,1,1,1};

ParallelBus busA;

////////////////////////////////////////////////
// LED indicators

void busy(){
  digitalWrite(PIN_LED, LED_ON);
}

void notBusy(){
  digitalWrite(PIN_LED, LED_OFF);
}


///////////////////////////////////////////////
// Temperature sensors
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);
DallasTemperatureSensor boardTemp(sensors, boardSensorAddress, "boardTemperature");
DallasTemperatureSensor boilerTemp(sensors, boilerSensorAddress, "boilerTemperature");
DallasTemperatureSensor feedTemp(sensors, feedSensorAddress, "feedTemperature");
DallasTemperatureSensor returnTemp(sensors, returnSensorAddress, "returnTemperature");
DallasTemperatureSensor outsideTemp(sensors, outsideSensorAddress,"outsideTemperature");
DallasTemperatureSensor insideTemp(sensors, insideSensorAddress, "insideTemperature");
DallasTemperatureSensor flueTemp(sensors, flueSensorAddress, "flueTemperature");
HighTemperatureSensor exhaustTemp(PIN_ANALOG_IN, "exhaustTemperature");

TemperatureSensor* tempSensors[] = {&boardTemp, &boilerTemp, &feedTemp, &returnTemp, &outsideTemp, &insideTemp, &flueTemp, &exhaustTemp}; 
int tempSensorsCount = sizeof(tempSensors) / sizeof(tempSensors[0]);  
  
////////////////////////////////////////////////
// Relays (bit 0 = relay 1, bit 1 = relay 2 etc)
Switch pumpRelay(busA, 0, "pump");
Switch pump2Relay(busA, 1, "pump2"); //Placeholder for now. This is a support for secondary pump
Switch coolerRelay(busA, 2, "cooler");
Switch heaterRelay1(busA, 3, "heater1");
Switch heaterRelay2(busA, 4, "heater2");
Switch heaterRelay3(busA, 5, "heater3");

Switch* switches[] = {
  &pumpRelay, &pump2Relay, 
  &heaterRelay1, &heaterRelay2, &heaterRelay3, 
  &coolerRelay
};
int switchesCount = sizeof(switches) / sizeof(switches[0]);

//////////////////////////////////////////////////
// Water flow control and heating power
FlowSensor flowSensor(PIN_DIGITAL_IO1);
HeatingPowerSensor heatingPowerSensor(&feedTemp, &returnTemp, &flowSensor);

//////////////////////////////////////////////////////////////////////////
// Servos

//TODO: Remove and write normal code
//Servo controller callback (A7, A8) - this is ugly small hack until libraries are extracted from this code
//Right now revos can be on busB only 
void ICACHE_RAM_ATTR bus_digitalWrite(uint8_t pin, uint8_t val){
  busA.setBit(pin, val == HIGH);
}

ServoController flueValve("flueValve", FLUE_VALVE_PIN, FLUE_VALVE_CLOSED_VALUE, FLUE_VALVE_OPEN_VALUE, FLUE_VALVE_SYNC_INTERVAL, FLUE_VALVE_ANTI_STALL_INTERVAL, FLUE_VALVE_ACTIVE_TIME);
ServoController boilerValve("boilerValve", BOILER_VALVE_PIN, BOILER_VALVE_CLOSED_VALUE, BOILER_VALVE_OPEN_VALUE, BOILER_VALVE_SYNC_INTERVAL, BOILER_VALVE_ANTI_STALL_INTERVAL, BOILER_VALVE_ACTIVE_TIME);
ServoController *valves[] = {&flueValve, &boilerValve};
int valvesCount = sizeof(valves) / sizeof(valves[0]);

Workflow workflow(
  &outsideTemp, &insideTemp, &boilerTemp, &flueTemp,
  &flowSensor, 
  &pumpRelay, &coolerRelay, 
  &heaterRelay1, &heaterRelay2, &heaterRelay3, 
  &flueValve, &boilerValve, 
  &heatingPowerSensor
);

///////////////////////////////////////////////////////////////////////////////////////////
// Controller setup and main loop

unsigned long lastBusSyncTime = 0;
void syncBus(){
  if (lastBusSyncTime == 0 || millis() - lastBusSyncTime > BUS_SYNC_DELAY){
    //Flush states to buses (to prevent them from random status changes due to interference)	
    busA.sync();
    lastBusSyncTime = millis();
  }
}

void setupBus(){
  //Init pins for communication with Parallel bus.
  pinMode(PIN_SET, OUTPUT);
  pinMode(PIN_CLK, OUTPUT);
  pinMode(PIN_DATA, OUTPUT);
  delay(500); //Give bus time to reset (this can be tuned)
  syncBus();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Code responsible for WiFi server funtionality

const int REGISTRATION_REFRESH_INTERVAL = 60000;
ConsulRegistration registration(SERVICE_NAME, REGISTRATION_REFRESH_INTERVAL);

const int SERVER_PORT = 80;
ESP8266WebServer server(SERVER_PORT);

bool isInWifi = false;
void processServer() {
  server.handleClient();
  if (isInWifi){
    registration.refresh();
  }
}

void onGotIP(const WiFiEventStationModeGotIP& event){
  DEBUG_PRINT_LN("Connected and got IP.");
	isInWifi = true;     
}

void onDisconnect(const WiFiEventStationModeDisconnected& event){
	isInWifi = false;
}

WiFiEventHandler onConnectHandler, onDisconnectHandler, onIPHandler;

void connectToWiFi() {
  onDisconnectHandler = WiFi.onStationModeDisconnected(onDisconnect);
  onIPHandler = WiFi.onStationModeGotIP(onGotIP);
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  DEBUG_PRINTF("Connecting to WiFi: %s\n", ssid);
}

#include "RequestHandlers.h" //TODO - restructure the implementation, remove this context-dependend include

void startServer() {
  DEBUG_PRINT_LN("HTTP server starting");
  MDNS.begin("boiler-control");
  server.on("/", handleGetRoot);
  
  server.on("/api/status", HTTP_GET, handleApiGetStatus);
  server.on("/api/status", HTTP_PUT, handleApiPutStatus);
  server.on("/api/config", HTTP_GET, handleApiGetConfig);
  server.on("/api/config", HTTP_PUT, handleApiPutConfig);
  server.on("/api/sensors", HTTP_GET, handleApiGetSensors);

  server.on("/status", HTTP_POST, handlePostStatus); //Ur for web form to work with status.
  
  server.on("/update", HTTP_GET, handleGetUpdate);
  server.on("/update", HTTP_POST, handlePostUpdateResult, handlePostUpdate);
  server.on("/reboot", HTTP_GET, handleGetReboot);
  server.on("/reboot", HTTP_POST, handlePostReboot);
  server.begin();
  DEBUG_PRINT_LN("HTTP server started");
}

void setupServer() {
  DEBUG_INIT();
  connectToWiFi();
  startServer();
}

void setup() {
  busy();

  DEBUG_INIT();
  pinMode(PIN_LED, OUTPUT);
  setupBus();
  setupServer();
  
  notBusy();
}

#ifndef DEBUG_MODE

void loop() {
  syncBus();
  flowSensor.syncSpeed();
  heatingPowerSensor.sync();
  workflow.sync();
  processServer();  
}

#else //DEBUG_MODE
#include "DebugMode.h"

void loop(){
  debugLoop();
}
#endif