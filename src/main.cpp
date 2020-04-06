#include <Arduino.h>
#include <ArduinoJson.h>

#include <EspDebug.h>
#include <ParallelBus.h>

#include <ESP8266WiFi.h>

#ifndef DEBUG_MODE
#include <OneWire.h> 
#include <DallasTemperature.h>
#include <EEPROM.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <BusServo.h>
#include <Switch.h>
#include <ConsulRegistration.h>
#include <FlowSensor.h>
#include <FlowSensor.h>
#include <TermoRelay.h>
#include <ConditionalRelay.h>
#include <ServoController.h>
#include <TemperatureSensor.h>
#endif //DEBUG_MODE

#include "WiFiCredentials.h"
#include "Settings.h"

//////////////////////////////////////////////
// Parallel bus
const int BUS_ADDR_A[] =   {0,0,1,0,0,0,0,0};
const int BUS_ADDR_B[] =   {0,1,0,0,0,0,0,0};

ParallelBus busA(BUS_ADDR_A, ADDR_LEN, STATE_LEN);
ParallelBus busB(BUS_ADDR_B, ADDR_LEN, STATE_LEN);

#ifndef DEBUG_MODE

//////////////////////////////////////////////////
//Temperature sensors addresses (do not forget to disable dev board sensors upon merge)

#ifdef DEBUG_BOARD

const char *SERVICE_NAME = "boiler-v2-dev";    //"-dev" indicates development board.
DeviceAddress boardSensorAddress   = {40, 255, 230, 219, 35, 23, 3, 139};   //DEV board sensor
DeviceAddress &boilerSensorAddress = boardSensorAddress;
DeviceAddress &feedSensorAddress   = boardSensorAddress;
DeviceAddress &returnSensorAddress  = boardSensorAddress;
DeviceAddress &outsideSensorAddress = boardSensorAddress;
DeviceAddress &insideSensorAddress  = boardSensorAddress;
DeviceAddress &flueSensorAddress  = boardSensorAddress;

#else //DEBUG_BOARD

const char *SERVICE_NAME = "boiler-v2";    
DeviceAddress boardSensorAddress   = {40, 255, 230, 219,  35,  23,  3, 139};   //Different for each board
DeviceAddress boilerSensorAddress  = {40,   0,   9,   0, 226,  56, 34, 161};
DeviceAddress feedSensorAddress    = {40,   0,   9,   0,   1, 110, 34,  95};
DeviceAddress returnSensorAddress  = {40,   0,   9,   0, 245,  57, 34,  85};
DeviceAddress outsideSensorAddress = {40, 255,  45, 233,  35,  23,  3, 150};
DeviceAddress insideSensorAddress  = {40, 255, 228,  13,  36,  23,  3, 122};
DeviceAddress flueSensorAddress  =   {40, 255, 232,  17,  36,  23,  3, 113};

#endif

//////////////////////////////////////////////////
// Water flow control
Switch flowSensorPower(busB, BUS_BIT_8, "flowSensor"); 
FlowSensor flowSensor(PIN_DIGITAL_IO1, &flowSensorPower);

////////////////////////////////////////////////
// LED indicators
Switch ledWiFi(busB, BUS_BIT_1, "ledWifi");     
Switch ledConsul(busB, BUS_BIT_2, "ledConsul"); 
Switch ledTemp(busB, BUS_BIT_3, "ledTemp");     
Switch ledBusy(busB, BUS_BIT_4, "ledBusy");     
Switch ledAlarm(busB, BUS_BIT_5, "ledAlarm");   

void busy(){
  ledBusy.turnOn();  
}

void notBusy(){
  ledBusy.turnOff();  
}


///////////////////////////////////////////////
// Temperature sensors
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);
TemperatureSensor boardTemp(sensors, boardSensorAddress, "boardTemperature", &ledTemp);
TemperatureSensor boilerTemp(sensors, boilerSensorAddress, "boilerTemperature", &ledTemp);
TemperatureSensor feedTemp(sensors, feedSensorAddress, "feedTemperature", &ledTemp);
TemperatureSensor returnTemp(sensors, returnSensorAddress, "returnTemperature", &ledTemp);
TemperatureSensor outsideTemp(sensors, outsideSensorAddress,"outsideTemperature", &ledTemp);
TemperatureSensor insideTemp(sensors, insideSensorAddress, "insideTemperature", &ledTemp);
TemperatureSensor flueTemp(sensors, flueSensorAddress, "flueTemperature", &ledTemp);

TemperatureSensor* tempSensors[] = {&boardTemp, &boilerTemp, &feedTemp, &returnTemp, &outsideTemp, &insideTemp, &flueTemp}; 
int tempSensorsCount = sizeof(tempSensors) / sizeof(tempSensors[0]);  
  
////////////////////////////////////////////////
// Relays (bit 0 = relay 1, bit 1 = relay 2 etc)
Switch pumpRelay(busA, BUS_BIT_1, "pump");       
Switch pump2Relay(busA, BUS_BIT_2, "pump2");
Switch heaterRelay1(busA, BUS_BIT_3, "heater1");
Switch heaterRelay2(busA, BUS_BIT_4, "heater2");
Switch heaterRelay3(busA, BUS_BIT_5, "heater3");
Switch coolerRelay(busA, BUS_BIT_6, "cooler");

TermoRelay pump(HEATING_PUMP_ON, HEATING_PUMP_OFF, &pumpRelay, &ledAlarm, true);
TermoRelay pumpNoFreeze(ANTIFREEZE_PUMP_ON, ANTIFREEZE_PUMP_OFF, &pumpRelay, &ledAlarm, true);
TermoRelay heater1(HEATER_1_ON, HEATER_1_OFF, &heaterRelay1, &ledAlarm); 
TermoRelay heater2(HEATER_2_ON, HEATER_2_OFF, &heaterRelay2, &ledAlarm); 
TermoRelay heater3(HEATER_3_ON, HEATER_3_OFF, &heaterRelay3, &ledAlarm); 

TermoRelay* termoRelays[] = {&pumpNoFreeze, &pump, &heater1, &heater2, &heater3}; 
int termoRelaysCount = sizeof(termoRelays) / sizeof(termoRelays[0]);

TermoRelay* boilerDependentTermoRelays[] = {&pumpNoFreeze, &pump, &heater1, &heater2, &heater3}; 
int boilerDependentTermoRelaysCount = sizeof(boilerDependentTermoRelays) / sizeof(boilerDependentTermoRelays[0]);

Switch* switches[] = {
  &pumpRelay, &pump2Relay, 
  &heaterRelay1, &heaterRelay2, &heaterRelay3, 
  &coolerRelay, 
  &flowSensorPower, 
  &ledWiFi, &ledConsul, &ledTemp, &ledBusy, &ledAlarm
};
int switchesCount = sizeof(switches) / sizeof(switches[0]);


bool calculateCoolerCondition(bool isOn){
  float boilerTemperature = boilerTemp.getTemperature();
  bool onForBoiler = boilerTemperature >= BOILER_COOLER_ON || (isOn && boilerTemperature > BOILER_COOLER_OFF);
  
  float flueTemperature = flueTemp.getTemperature();
  bool onForFlue = flueTemperature >= FLUE_COOLER_ON || (isOn && flueTemperature > FLUE_COOLER_OFF); 

  return onForBoiler || onForFlue; 
}
ConditionalRelay cooler(&coolerRelay, calculateCoolerCondition); 

//////////////////////////////////////////////////////////////////////////
// Servos

//TODO: Remove and write normal code
//Servo controller callback (B6, B7, B8) - this is ugly small hack until libraries are extracted from this code
//Right now revos can be on busB only 
void ICACHE_RAM_ATTR bus_digitalWrite(uint8_t pin, uint8_t val){
  busB.setBit(pin, val == HIGH);
}

float calculateCurrentPower(){
  //P = c*(T1-T0)*dm/dt
  return 4.2 * (feedTemp.getTemperature() - returnTemp.getTemperature()) * flowSensor.getLitresPerMinute() / 60.0;
}

int calculateFlueValveValue(){
  float temperature = max(flueTemp.getTemperature(), boilerTemp.getTemperature());
  if (temperature < -100){
    return 40; //Default mode - temperature sensor is not working
  }
  else if (temperature <= 20){
    return 0;  
  }
  else if (temperature >= 100){
    return 80;  
  }
  else{ 
    return 80 - 80 * (1 - (temperature - 20) / (100.0 - 20.0)); //Linear scale between 20 and 100 celsius (20 = 100%, 100 = 0%)  
  } 
}

int calculateBoilerValveValue(){
  //Decide boiler temperature from the outside temperature
  const int minTemperatureValue = 20; //Servo position with min temperature
  const int maxTemperatureValue = 100; //Servo position with max temperature
  const int maxOutsideTemperature = 10;
  const int minOutsideTemperature = -20;
  float temperature = outsideTemp.getTemperature();
  if (temperature < -100){
    return minTemperatureValue; //Default mode - temperature sensor is not working, so, do not overheat
  }
  else if (temperature >= maxOutsideTemperature){
    return minTemperatureValue;  //Edge case - the warmest weather supported
  }
  else if (temperature <= minOutsideTemperature){
    return maxTemperatureValue;         //Edge case - the coldest weather supported
  }
  else{
    return minTemperatureValue + (maxTemperatureValue - minTemperatureValue) * (maxOutsideTemperature - temperature) / (maxOutsideTemperature - minOutsideTemperature); //linear scale between min and max values
  }
}

ServoController flueValve("flueValve", FLUE_VALVE_PIN, FLUE_VALVE_SYNC_INTERVAL, FLUE_VALVE_ANTI_STALL_INTERVAL, FLUE_VALVE_ACTIVE_TIME, calculateFlueValveValue);
ServoController boilerValve("boilerValve", BOILER_VALVE_PIN, BOILER_VALVE_SYNC_INTERVAL, BOILER_VALVE_ANTI_STALL_INTERVAL, BOILER_VALVE_ACTIVE_TIME, calculateBoilerValveValue);
ServoController *valves[] = {&flueValve, &boilerValve};
int valvesCount = sizeof(valves) / sizeof(valves[0]);

///////////////////////////////////////////////////////////////////////////////////////////
// Controller setup and main loop

unsigned long lastPumpCheckTime = 0;
void checkPumpRelay(){
  if (lastPumpCheckTime == 0 || millis() - lastPumpCheckTime > PUMP_CHECK_DELAY){
    if (pumpRelay.isOn() && flowSensor.getTicks() > 0 && flowSensor.getLitresPerMinute() < 0.01){
      //Pump is supposed to be on, but there is no flow, pump relay might be reset. Need to switch it on again.
      ledAlarm.turnOn();
      pumpRelay.turnOff();
      delay(500);
      pumpRelay.turnOn();
      ledAlarm.turnOff();
    }
    lastPumpCheckTime = millis();
  }
}

unsigned long lastBusSyncTime = 0;
void syncBus(){
  if (lastBusSyncTime == 0 || millis() - lastBusSyncTime > BUS_SYNC_DELAY){
    //Flush states to buses (to prevent them from random status changes due to interference)	
    busA.sync();
    busB.sync();
    lastBusSyncTime = millis();
  }
}

unsigned long lastSyncTime = 0;
void syncTermoRelays(){
  if (lastSyncTime == 0 || millis() - lastSyncTime > TERMO_SYNC_DELAY){
    
    //Relays, dependent on builer temperature.
    float currentBoilerTemp = boilerTemp.getTemperature();
    for (int i=0; i<boilerDependentTermoRelaysCount; i++){
      boilerDependentTermoRelays[i]->processTemp(currentBoilerTemp);
    }

    //Conditional relays.
    cooler.sync();
    
    lastSyncTime = millis();
  }
}

void syncValves(){
  for (int i=0; i<valvesCount; i++){
    valves[i]->sync();
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
ConsulRegistration registration(SERVICE_NAME, REGISTRATION_REFRESH_INTERVAL, &ledWiFi, &ledConsul);

const int SERVER_PORT = 80;
ESP8266WebServer server(SERVER_PORT);

void processServer() {
  server.handleClient();
  registration.refresh();
}

void connectToWiFi() {
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
  DEBUG_INIT();
  setupBus();
  setupServer();
}

void loop() {
  syncTermoRelays();  
  flowSensor.syncSpeed(); 
  processServer();  
  syncValves();
  syncBus();
  checkPumpRelay();
}

#else //DEBUG_MODE
#include "DebugMode.h"

void setup() {
  debugSetup();
}

void loop(){
  debugLoop();
}
#endif