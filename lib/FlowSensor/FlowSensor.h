#ifndef __FLOW_SENSOR__INCLUDED__
#define __FLOW_SENSOR__INCLUDED__

#include <Switch.h>

class FlowSensor {
  private:
    const float LITRES_PER_TICK = 0.005; //Results of calibration. See excel sheet.
    const unsigned int SPEED_MEASUREMENT_INTERVAL = 60000;
    
    unsigned long lastSpeedMeasurement = 0;
    unsigned long lastTicks = 0;
    long ticksPerMinute = 0;
      
  public:
    FlowSensor(int interruptPin, Switch *powerSwitch);

    void syncSpeed();

    unsigned long getTicks();

    float getLitresTotal();

    float getLitresPerMinute();
};

#endif //__FLOW_SENSOR__INCLUDED__