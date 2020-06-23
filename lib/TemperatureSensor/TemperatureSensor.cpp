#include "TemperatureSensor.h"
#include "../../include/Settings.h"

TemperatureSensor::TemperatureSensor(DallasTemperature &sensors, DeviceAddress &sensorAddress, const char *sensorName){
  this->sensors = &sensors;
  this->sensorAddress = &sensorAddress;
  this->sensorName = sensorName;
}

float TemperatureSensor::getTemperature(){
  if (lastMeasurement == 0 || millis() - lastMeasurement > TERMO_SYNC_DELAY){
    ensureStarted();
    float temp = measureTemp(); 
    if (temp == ERROR_TEMP_VALUE){
      resetSensors(); //Reset once in case sensors hang. But only once to ensure, that the value is not the actual measurement.
      temp = measureTemp();
    }
    lastTempValue = round(temp * 10.0) / 10.0; //resolution is 1 degree after zero
    lastMeasurement = millis();
  }
  return lastTempValue;
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