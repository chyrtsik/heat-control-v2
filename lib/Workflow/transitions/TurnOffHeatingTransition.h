#ifndef __TURN_OFF_HEATING_TRANSITION__INCLUDED__
#define __TURN_OFF_HEATING_TRANSITION__INCLUDED__

#include "../WorkflowTransition.h"
#include <TemperatureSensor.h>
#include <FlowSensor.h>
#include <HeatingPowerSensor.h>
#include "TurnOnHeatingTransition.h"

class TurnOffHeatingTransition : public WorkflowTransition
{
  public:
    TemperatureSensor *outside;
    FlowSensor *flow;
    HeatingPowerSensor *heatingPower;
    TurnOnHeatingTransition *turnOnHeating;

    TurnOffHeatingTransition(TemperatureSensor *outside, FlowSensor *flow, HeatingPowerSensor *heatingPower, TurnOnHeatingTransition *turnOnHeating){
      this->outside = outside;
      this->flow = flow;
      this->heatingPower = heatingPower;
      this->turnOnHeating = turnOnHeating;
    }

  private: 
    bool isTooHot(){
      return outside->getTemperature() >= 20;   //TODO - make this configurable
    }

    bool isWarmEnough(){
      return outside->getTemperature() >= 10;   //TODO - make this configurable
    }

    bool isTooLittleHeatingPower(){
      return 
        heatingPower->getPower_1m() < heatingPower->getPower_15m() && 
        heatingPower->getPower_15m() < 0.5 && 
        heatingPower->getPower_15m() < heatingPower->getPower_1h() &&
        heatingPower->getPower_1h() < heatingPower->getPower_24h();
    }

  public:
    const char* getName(){
      return "TurnOffHeating";
    }

    bool canHappen(){
      //Checking for turn on prevents edge case, when both conditions can happen at the same time. 
      return !turnOnHeating->canHappen() && (isTooHot() || (isWarmEnough() && isTooLittleHeatingPower()));
    }
};

#endif //__TURN_ON_HEATING_TRANSITION__INCLUDED__