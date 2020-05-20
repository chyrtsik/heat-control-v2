#ifndef __PUMP_CHECKER__INCLUDED__
#define __PUMP_CHECKER__INCLUDED__

#include <EspDebug.h>
#include <Switch.h>
#include <FlowSensor.h>

class PumpChecker {
  private:
    Switch *pump;
    FlowSensor *flow;
  
    const unsigned long PUMP_CHECK_DELAY  = 10000;       //Delay between checks if pump relay is in a correct state
    const unsigned long PUMP_CHECK_RESET_DELAY  = 1000;  //Delay to reset pump state if pump shows no flow
    unsigned long lastPumpCheckTime = 0, resetStart = 0; 
    bool isResetting = false;
  public:
    PumpChecker(Switch *pump, FlowSensor *flow){
      this->pump = pump;
      this->flow = flow;
    }

    void check(){
      unsigned long currentTime = millis();
      if (isResetting){
        if (currentTime - resetStart > PUMP_CHECK_RESET_DELAY){
          //Time to finish pump reset
          isResetting = false;
          pump->turnOn();
          lastPumpCheckTime = currentTime;
          DEBUG_PRINT_LN("PumpChecker: Finished pump restart");
        }
      }
      else if (currentTime - lastPumpCheckTime > PUMP_CHECK_DELAY){
        if (pump->isOn() && flow->getLitresPerMinute() < 0.01){
          //Pump is supposed to be on, but there is no flow, pump relay might be reset. Need to switch it on again (will turn off and turn on with delay).
          DEBUG_PRINT_LN("PumpChecker: Starting pump restart");
          isResetting = true;
          pump->turnOff();
          resetStart = currentTime;
        }
      }
    }

    bool isBusy(){
      return isResetting;
    }
};

#endif //__PUMP_CHECKER__INCLUDED__