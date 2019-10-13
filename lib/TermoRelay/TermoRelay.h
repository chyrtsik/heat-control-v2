#ifndef __TERMO_RELAY__INCLUDED__
#define __TERMO_RELAY__INCLUDED__

#include <Switch.h> 

class TermoRelay{
  private:
    float onTemp;
    float offTemp;
    Switch *relay;
    Switch *ledAlarm;
    boolean stateOnError; //Should relay be on or off in case of temperature sensor error
    
  public:
    TermoRelay(float onTemp, float offTemp, Switch *relay, Switch *ledAlarm, boolean stateOnError);

    TermoRelay(float onTemp, float offTemp, Switch *relay, Switch *ledAlarm);

    boolean isOn();

    const char *getName();

    float getOnTemp();

    float getOffTemp();

    void processTemp(float currentTemp);
    
  private: 
    boolean isValidWaterTemp(float temperature);
};

#endif //__TERMO_RELAY__INCLUDED__