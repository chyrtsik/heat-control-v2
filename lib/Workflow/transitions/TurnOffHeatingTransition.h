#ifndef __TURN_OFF_HEATING_TRANSITION__INCLUDED__
#define __TURN_OFF_HEATING_TRANSITION__INCLUDED__

#include "../WorkflowTransition.h"
#include <TemperatureSensor.h>
#include <FlowSensor.h>

typedef float (*HeatingPowerSupplier)();

class TurnOffHeatingTransition : public WorkflowTransition
{
  public:
    TemperatureSensor *outside;
    HeatingPowerSupplier heatingPower;

    TurnOffHeatingTransition(TemperatureSensor *outside, HeatingPowerSupplier heatingPower){
      this->outside = outside;
      this->heatingPower = heatingPower;
    }

  private: 
    bool isTooHot(){
      return outside->getTemperature() > 20; //TODO - make this configurable
    }

    bool isWarmEnough(){
      return outside->getTemperature() > 15; //TODO - make this configurable
    }

    bool isTooLittleHeatingPower(){
      return heatingPower() < 0.2; //TODO - make configurable
    }

  public:
    const char* getName(){
      return "TurnOffHeatingTransition";
    }

    bool canHappen(){
      return isTooHot() || (isWarmEnough() ? isTooLittleHeatingPower() : false);
    }
};

#endif //__TURN_ON_HEATING_TRANSITION__INCLUDED__