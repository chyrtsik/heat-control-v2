#ifndef __TURN_ON_HEATING_TRANSITION__INCLUDED__
#define __TURN_ON_HEATING_TRANSITION__INCLUDED__

#include "../WorkflowTransition.h"
#include "../common/PumpFlowMeasurer.h"
#include <TemperatureSensor.h>
#include <FlowSensor.h>
#include <Switch.h>

#define TURN_ON_HEATING_TIME_BETWEEN_FLOW_CHECKS    7200000   //Check pump each 2 hours  
#define TURN_ON_HEATING_FLOW_CHECK_DURATION         30000     //Measure flow for long enough
#define TURN_ON_HEATING_FLOW_THRESHOLD              13.0      //Value above this trigger heating enabling (some of heating devices are open for heating) 
#define TURN_ON_HEATING_TRY_TEMPERATURE_THRESHOLD   16.0      //Min temperature to start checking the need for heating
#define TURN_ON_HEATING_TRY_INSITE_TEMPERATURE_THRESHOLD   22.0      //Inside temperature to try heating
#define TURN_ON_HEATING_FORCE_TEMPERATURE_THRESHOLD 1.0       //Min temperature to start heating unconditionally

class TurnOnHeatingTransition : public WorkflowTransition
{
  public:
    TemperatureSensor *outside, *inside;
    PumpFlowMeasurer pumpFlowMeasurer;

    TurnOnHeatingTransition(TemperatureSensor *outside, TemperatureSensor *inside, FlowSensor *flow, Switch *pump) 
    : pumpFlowMeasurer(pump, flow, TURN_ON_HEATING_TIME_BETWEEN_FLOW_CHECKS, TURN_ON_HEATING_FLOW_CHECK_DURATION)
    {
      this->outside = outside;
      this->inside = inside;
    }

  private:
    bool isTooCold(){
      return outside->getTemperature() < TURN_ON_HEATING_FORCE_TEMPERATURE_THRESHOLD;
    }

    bool isColdENoughToTryFlow(){
      return outside->getTemperature() < TURN_ON_HEATING_TRY_TEMPERATURE_THRESHOLD && inside->getTemperature() < TURN_ON_HEATING_TRY_INSITE_TEMPERATURE_THRESHOLD;
    }

    bool hasEnoughWaterFlow(){
        return pumpFlowMeasurer.getLastMeasuredFlow() > TURN_ON_HEATING_FLOW_THRESHOLD; 
    }

  public:
    const char* getName(){
      return "TurnOnHeating";
    }

    bool canHappen(){
      return isTooCold() || (isColdENoughToTryFlow() ? hasEnoughWaterFlow() : false);
    }
};

#endif //__TURN_ON_HEATING_TRANSITION__INCLUDED__