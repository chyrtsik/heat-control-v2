#ifndef __TEMPERATURE_SENSOR__INCLUDED__
#define __TEMPERATURE_SENSOR__INCLUDED__

#include <Switch.h>
#include <DallasTemperature.h>

class TemperatureSensor{
  private:
    DallasTemperature *sensors;
    DeviceAddress *sensorAddress;
    const char *sensorName;
    Switch *ledTemp;

    float lastTempValue;
    unsigned long lastMeasurement = 0;
    
  public:
    TemperatureSensor(DallasTemperature &sensors, DeviceAddress &sensorAddress, const char *sensorName, Switch *ledTemp);

    float getTemperature();

    const char* getName();

  private:
    float measureTemp();

  private: 
    void ensureStarted();

    void resetSensors();
};

#endif //__TEMPERATURE_SENSOR__INCLUDED__