#ifndef __IDLE_STATE__INCLUDED__
#define __IDLE_STATE__INCLUDED__

#include "../WorkflowState.h"
#include "../common/PumpFlowMeasurer.h"

#include <ServoController.h>

class IdleState : public WorkflowState
{ 
  private:  
    ServoController *flueServo, *boilerServo;
    PumpFlowMeasurer pumpFlowMeasurer;

  public:
    IdleState(ServoController *flueServo, ServoController *boilerServo, Switch *pump, FlowSensor *flow)
    : pumpFlowMeasurer(pump, flow, IDLE_TIME_BETWEEN_FLOW_CHECKS, IDLE_FLOW_CHECK_DURATION)
    {
      this->flueServo = flueServo;
      this->boilerServo = boilerServo;
    }

    const char* getName(){ 
      return "Idle"; 
    }

    void printStatus(JsonObject &stateJsonNode){
      if (!pumpFlowMeasurer.isBusy()){
        float lastMeasuredFlow = pumpFlowMeasurer.getLastMeasuredFlow();
        stateJsonNode["pumpIsHealthy"] = lastMeasuredFlow >= IDLE_PUMP_HEALTHY_FLOW_THRESHOLD;
        stateJsonNode["pumpLastMeasuredFlow"] = lastMeasuredFlow;
      }
    }

    bool canEnter(){
      return true;
    }

    void onEnter(){
      flueServo->setValue(FLUE_VALVE_CLOSED_VALUE);
      boilerServo->setValue(BOILER_VALVE_CLOSED_VALUE);
    }
    
    void sync(){
      //Check pump periodically (once a week or so)
      pumpFlowMeasurer.getLastMeasuredFlow();  //Do nothing with it - calculation requires flow triggering

      //TODO - check heaters periodically (once a 1-2 months)

      //Ensure that valves do not stuck 
      flueServo->syncAntiStall();
      boilerServo->syncAntiStall();
    }
    
    bool canExit(){
      //Override in case there will be operation which cannot be cancelled.
      return !pumpFlowMeasurer.isBusy();
    }

    void onExit(){
    }
};

#endif //__IDLE_STATE__INCLUDED__