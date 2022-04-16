#ifndef __OVER_HEATING_ERROR_TRANSITION__INCLUDED__
#define __OVER_HEATING_ERROR_TRANSITION__INCLUDED__

#include "../../WorkflowTransition.h"
#include <TemperatureSensor.h>
#include <FlowSensor.h>

#define ERROR_BOILER_OVER_HEATING_THRESHOLD 85.0  //Min temperature above which boiler is considered overheated

class OverHeatingErrorTransition : public WorkflowTransition
{
  public:
    TemperatureSensor *boiler;

    OverHeatingErrorTransition(TemperatureSensor *boiler){
      this->boiler = boiler;
    }

  public:
    const char* getName(){
      return "BoilerOverheating";
    }

    bool canHappen(){
      return boiler->getTemperature() > ERROR_BOILER_OVER_HEATING_THRESHOLD; 
    }
};

#endif //__OVER_HEATING_ERROR_TRANSITION__INCLUDED__