#ifndef __PUMP_FAILURE_ERROR_TRANSITION__INCLUDED__
#define __PUMP_FAILURE_ERROR_TRANSITION__INCLUDED__

#include "../../WorkflowTransition.h"
#include <TemperatureSensor.h>
#include <FlowSensor.h>
#include <Switch.h>

#define PUMP_FAILURE_FLOW_THRESHOLD 1.0     //Min flow which is valid for pump
#define PUMP_FAILURE_MIN_TIME       10000   //Min time pump should be active to check for error
#define PUMP_FAILURE_EXPIRATION     5000    //Min time to expire error (so that short pump turn on / off do not disable this error)   

class PumpFailureErrorTransition : public WorkflowTransition
{
    Switch *pump;
    FlowSensor *flow;

    bool isTriggered = false;

  public:
    PumpFailureErrorTransition(Switch *pump, FlowSensor *flow)
    {
      this->pump = pump;
      this->flow = flow;
    }

    const char* getName(){
      return "PumpFailureError";
    }

    bool canHappen(){
      if (pump->isOn()){
        if (pump->getTimeInLastState() >= PUMP_FAILURE_MIN_TIME){
          if (flow->getLitresPerMinute() < PUMP_FAILURE_FLOW_THRESHOLD){
            //Pump is not working (it was supposed to be a significant flow)
            isTriggered = true;
          }
          else{
            //Pump is working
            isTriggered = false;
          }
        }
      } 
      else{
        if (isTriggered && pump->getTimeInLastState() >= PUMP_FAILURE_EXPIRATION) {
          //Error has expired
          isTriggered = false;
        }
      }
      return isTriggered;
    }
};

#endif //__PUMP_FAILURE_ERROR_TRANSITION__INCLUDED__