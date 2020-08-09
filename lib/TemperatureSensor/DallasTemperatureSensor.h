#ifndef __DALLAS_TEMPERATURE_SENSOR__INCLUDED__
#define __DALLAS_TEMPERATURE_SENSOR__INCLUDED__

#include "TemperatureSensor.h"
#include <DallasTemperature.h>

class DallasTemperatureSensor : public TemperatureSensor {
  private:
    DallasTemperature *sensors;
    DeviceAddress *sensorAddress;

  public:
    DallasTemperatureSensor(DallasTemperature &sensors, DeviceAddress &sensorAddress, const char *sensorName) : TemperatureSensor(sensorName){
      this->sensors = &sensors;
      this->sensorAddress = &sensorAddress;
    }

  protected:
    float measureTemp(){
      ensureStarted();
      float temp = receiveSensorTemp(); 
      if (temp == ERROR_TEMP_VALUE){
        resetSensors(); //Reset once in case sensors hang. But only once to ensure, that the value is not the actual measurement.
        temp = receiveSensorTemp();
      }
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
      return sensors->getTempC(*sensorAddress);
    }
 
};

#endif //__DALLAS_TEMPERATURE_SENSOR__INCLUDED__