#include "TemperatureSensor.h"
#include "../../include/Settings.h"

TemperatureSensor::TemperatureSensor(DallasTemperature &sensors, DeviceAddress &sensorAddress, const char *sensorName, Switch *ledTemp){
  this->sensors = &sensors;
  this->sensorAddress = &sensorAddress;
  this->sensorName = sensorName;
  this->ledTemp = ledTemp;
}

float TemperatureSensor::getTemperature(){
  ledTemp->turnOn();
  ensureStarted();
  float temp = measureTemp(); 
  if (temp == ERROR_TEMP_VALUE){
    resetSensors(); //Reset once in case sensors hang. But only once to ensure, that the value is not the actual measurement.
    temp = measureTemp();
  }
  ledTemp->turnOff();
  return round(temp * 10.0) / 10.0; //resolution is 1 degree after zero
}

const char* TemperatureSensor::getName(){
  return sensorName;
}

float TemperatureSensor::measureTemp(){
  sensors->requestTemperaturesByAddress(*sensorAddress);
  return sensors->getTempC(*sensorAddress);
}

void TemperatureSensor::ensureStarted(){
  static boolean started = false;    
  if (!started){
    started = true;
    resetSensors();
  }
}

void TemperatureSensor::resetSensors(){
  sensors->begin(); 
  sensors->setResolution(TEMPERATURE_PRECISION);
}