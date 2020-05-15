#ifndef __TEMPERATURE_SENSOR_ERROR_TRANSITION__INCLUDED__
#define __TEMPERATURE_SENSOR_ERROR_TRANSITION__INCLUDED__

#include "../WorkflowTransition.h"
#include <TemperatureSensor.h>
#include <FlowSensor.h>

class TemperatureSensorErrorTransition  : public WorkflowTransition
{
  public:
    TemperatureSensor *boiler;

    TemperatureSensorErrorTransition(TemperatureSensor *boiler){
      this->boiler = boiler;
    }

  public:
    const char* getName(){
      return "TemperatureSensorError";
    }

    bool canHappen(){
      return boiler->getTemperature() < 0; //Either sensor is not working (it returns -127 in case of error) or boiler is about to freese.
    }
};

#endif //__TEMPERATURE_SENSOR_ERROR_TRANSITION__INCLUDED__