#ifndef __HEATING_POWER_SENSOR__INCLUDED__
#define __HEATING_POWER_SENSOR__INCLUDED__

#include <TemperatureSensor.h>
#include <FlowSensor.h>

#define POWER_MEASUREMENT_INTERVAL 10000
#define POWER_1M_INTERVAL          60000.0
#define POWER_15M_INTERVAL        900000.0
#define POWER_1H_INTERVAL        3600000.0
#define POWER_24H_INTERVAL      86400000.0
#define POWER_7D_INTERVAL      604800000.0

class HeatingPowerSensor{
  private:
    TemperatureSensor *feed, *ret;
    FlowSensor *flow;

    //Heating power immediate and moving average accross multiple durations
    float power, power_1m, power_15m, power_1h, power_24h, power_7d; 
    
    unsigned long lastMeasurementTime = 0;
  
  public:
      HeatingPowerSensor(TemperatureSensor *feed, TemperatureSensor *ret, FlowSensor *flow){
        this->feed = feed;
        this->ret = ret;
        this->flow  = flow;
      }

      void sync(){
        if (lastMeasurementTime == 0 || millis() - lastMeasurementTime >= POWER_MEASUREMENT_INTERVAL){
          power = calculateCurrentPower();
          power_1m = (POWER_MEASUREMENT_INTERVAL / POWER_1M_INTERVAL) * power + ((POWER_1M_INTERVAL - POWER_MEASUREMENT_INTERVAL) / POWER_1M_INTERVAL) * power_1m;
          power_15m = (POWER_MEASUREMENT_INTERVAL / POWER_15M_INTERVAL) * power + ((POWER_15M_INTERVAL - POWER_MEASUREMENT_INTERVAL) / POWER_15M_INTERVAL) * power_15m;
          power_1h = (POWER_MEASUREMENT_INTERVAL / POWER_1H_INTERVAL) * power + ((POWER_1H_INTERVAL - POWER_MEASUREMENT_INTERVAL) / POWER_1H_INTERVAL) * power_1h;
          power_24h = (POWER_MEASUREMENT_INTERVAL / POWER_24H_INTERVAL) * power + ((POWER_24H_INTERVAL - POWER_MEASUREMENT_INTERVAL) / POWER_24H_INTERVAL) * power_24h;
          power_7d = (POWER_MEASUREMENT_INTERVAL / POWER_7D_INTERVAL) * power + ((POWER_7D_INTERVAL - POWER_MEASUREMENT_INTERVAL) / POWER_7D_INTERVAL) * power_7d;
          lastMeasurementTime = millis();
        }
      }

      float getPower(){
        return power;
      }

      float getPower_1m(){
        return power_1m;
      }
      
      float getPower_15m(){
        return power_15m;
      }

      float getPower_1h(){
        return power_1h;
      }

      float getPower_24h(){
        return power_24h;
      }

      float getPower_7d(){
        return power_7d;
      }

  private:

    float calculateCurrentPower(){
      //P = c*(T1-T0)*dm/dt
      return 4.2 * (feed->getTemperature() - ret->getTemperature()) * flow->getLitresPerMinute() / 60.0;
    }
       
};

#endif //__HEATING_POWER_SENSOR__INCLUDED__