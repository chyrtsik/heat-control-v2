#ifndef __TURN_ON_HEATING_TRANSITION__INCLUDED__
#define __TURN_ON_HEATING_TRANSITION__INCLUDED__

#include "../WorkflowTransition.h"
#include "../common/PumpFlowMeasurer.h"
#include <TemperatureSensor.h>
#include <FlowSensor.h>
#include <Switch.h>

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
      if (pumpFlowMeasurer.isBusy()){
        pumpFlowMeasurer.getLastMeasuredFlow(); //Just keep measurement until it is done
        return false;
      }
      return isTooCold() || (isColdENoughToTryFlow() ? hasEnoughWaterFlow() : false);
    }
};

#endif //__TURN_ON_HEATING_TRANSITION__INCLUDED__