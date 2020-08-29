#ifndef __TURN_OFF_HEATING_TRANSITION__INCLUDED__
#define __TURN_OFF_HEATING_TRANSITION__INCLUDED__

#include "../WorkflowTransition.h"
#include <TemperatureSensor.h>
#include <FlowSensor.h>
#include <HeatingPowerSensor.h>

class TurnOffHeatingTransition : public WorkflowTransition
{
  public:
    TemperatureSensor *outside;
    FlowSensor *flow;
    HeatingPowerSensor *heatingPower;

    TurnOffHeatingTransition(TemperatureSensor *outside, FlowSensor *flow, HeatingPowerSensor *heatingPower){
      this->outside = outside;
      this->flow = flow;
      this->heatingPower = heatingPower;
    }

  private: 
    bool isTooHot(){
      return outside->getTemperature() >= 20;   //TODO - make this configurable
    }

    bool isWarmEnough(){
      return outside->getTemperature() >= 15;   //TODO - make this configurable
    }

    bool isTooLittleHeatingPower(){
      return heatingPower->getPower() < 0.2;    //TODO - make configurable
    }

    bool isTooLittleFlow(){
      return flow->getLitresPerMinute() < 9.0;  //TODO - make configurable
    }

  public:
    const char* getName(){
      return "TurnOffHeating";
    }

    bool canHappen(){
      return isTooHot() || (isWarmEnough() && isTooLittleHeatingPower() && isTooLittleFlow());
    }
};

#endif //__TURN_ON_HEATING_TRANSITION__INCLUDED__