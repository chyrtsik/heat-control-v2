#ifndef __UNEXPECTED_HEATING_ERROR_TRANSITION__INCLUDED__
#define __UNEXPECTED_HEATING_ERROR_TRANSITION__INCLUDED__

#include "../../WorkflowTransition.h"
#include "../TurnOnHeatingTransition.h"
#include <TemperatureSensor.h>

class UnexpectedHeatingErrorTransition : public WorkflowTransition
{
  public:
    TemperatureSensor *boiler, *flue;
    TurnOnHeatingTransition *turnOnHeatingTransition;

    UnexpectedHeatingErrorTransition(TemperatureSensor *boiler, TemperatureSensor *flue, TurnOnHeatingTransition *turnOnHeatingTransition){
      this->boiler = boiler;
      this->flue = flue;
      this->turnOnHeatingTransition = turnOnHeatingTransition;
    }

  public:
    const char* getName(){
      return "UnexpectedHeatingError";
    }

    bool canHappen(){
      return (boiler->getTemperature() > 50 || flue->getTemperature() > 70) ? !turnOnHeatingTransition->canHappen() : false; 
    }
};

#endif //__UNEXPECTED_HEATING_ERROR_TRANSITION__INCLUDED__