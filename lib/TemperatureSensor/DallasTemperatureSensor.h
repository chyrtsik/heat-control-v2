#ifndef __DALLAS_TEMPERATURE_SENSOR__INCLUDED__
#define __DALLAS_TEMPERATURE_SENSOR__INCLUDED__

#include "TemperatureSensor.h"
#include <DallasTemperature.h>

#define TEMP_NAN_VALUE -99999.0
#define TEMP_MAX_DIFF 5.0         //Max supported difference between subsequent temperature measurements

class DallasTemperatureSensor : public TemperatureSensor {
  private:
    DallasTemperature *sensors;
    DeviceAddress *sensorAddress;
    float offset;
    float lastTemp = TEMP_NAN_VALUE;

  public:
    DallasTemperatureSensor(DallasTemperature &sensors, DeviceAddress &sensorAddress, const char *sensorName) : TemperatureSensor(sensorName){
      this->sensors = &sensors;
      this->sensorAddress = &sensorAddress;
      this->offset = 0;
    }

    DallasTemperatureSensor(DallasTemperature &sensors, DeviceAddress &sensorAddress, const char *sensorName, float offset) : TemperatureSensor(sensorName){
      this->sensors = &sensors;
      this->sensorAddress = &sensorAddress;
      this->offset = offset;
    }

  protected:
    float measureTemp(){
      ensureStarted();
      float temp = receiveSensorTemp();
      if (lastTemp == TEMP_NAN_VALUE){
          lastTemp = temp;
      } 
      if (temp == ERROR_TEMP_VALUE || fabs(temp - lastTemp) > TEMP_MAX_DIFF){
        resetSensors(); //Reset once in case sensors hang. But only once to ensure, that the value is not the actual measurement.
        temp = receiveSensorTemp();
      }
      lastTemp = temp;
      return temp;
    }

  private:
    void ensureStarted(){
      static boolean started = false;    
      if (!started){
        started = true;
        resetSensors();
      }
    }

    void resetSensors(){
      sensors->begin(); 
      sensors->setResolution(TEMPERATURE_PRECISION);
    }

    float receiveSensorTemp(){
      sensors->requestTemperaturesByAddress(*sensorAddress);
      return sensors->getTempC(*sensorAddress) + offset;
    }
 
};

#endif //__DALLAS_TEMPERATURE_SENSOR__INCLUDED__