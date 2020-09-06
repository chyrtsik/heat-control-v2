#ifndef __TEMPERATURE_SENSOR__INCLUDED__
#define __TEMPERATURE_SENSOR__INCLUDED__

#include <Settings.h>

class TemperatureSensor{
  private:
    const char *sensorName;
    
    float lastTempValue;
    unsigned long lastMeasurement = 0;
    
  public:
    TemperatureSensor(const char *sensorName){
      this->sensorName = sensorName;
    }

    float getTemperature(){
      if (lastMeasurement == 0 || millis() - lastMeasurement > TERMO_SYNC_DELAY){
        float temp = measureTemp();
        lastTempValue = lastMeasurement == 0 ? temp : lastTempValue * 0.9 + temp * 0.1;
        lastTempValue = round(lastTempValue * 10.0) / 10.0; //resolution is 1 degree after zero
        lastMeasurement = millis();
      }
      return lastTempValue;
    }

    const char* getName(){
      return sensorName;
    }

  protected:
    virtual float measureTemp() = 0;
};

#endif //__TEMPERATURE_SENSOR__INCLUDED__