#ifndef __TURN_ON_HEATING_TRANSITION__INCLUDED__
#define __TURN_ON_HEATING_TRANSITION__INCLUDED__

#include "../WorkflowTransition.h"
#include "../common/PumpFlowMeasurer.h"
#include <TemperatureSensor.h>
#include <FlowSensor.h>
#include <Switch.h>

#define TURN_ON_HEATING_TIME_BETWEEN_FLOW_CHECKS 3600000   //Check pump each hour  
#define TURN_ON_HEATING_FLOW_CHECK_DURATION      30000     //Measure flow for long enough

class TurnOnHeatingTransition : public WorkflowTransition
{
  public:
    TemperatureSensor *outside;
    PumpFlowMeasurer pumpFlowMeasurer;

    TurnOnHeatingTransition(TemperatureSensor *outside, FlowSensor *flow, Switch *pump) 
    : pumpFlowMeasurer(pump, flow, TURN_ON_HEATING_TIME_BETWEEN_FLOW_CHECKS, TURN_ON_HEATING_FLOW_CHECK_DURATION)
    {
      this->outside = outside;
    }

  private:
    bool isTooCold(){
      return outside->getTemperature() < 0; //TODO - make this configurable
    }

    bool isColdENoughToTryFlow(){
      return outside->getTemperature() < 15; //TODO - make this configurable
    }

    bool hasEnoughWaterFlow(){
        return pumpFlowMeasurer.getLastMeasuredFlow() > 10.0; //TODO - make this configurable
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