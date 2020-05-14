#ifndef __WORKFLOW__INCLUDED__
#define __WORKFLOW__INCLUDED__

#include <map>
#include <list>
#include <EspDebug.h>

#include "WorkflowState.h"
#include "WorkflowTransition.h"

#include "states/IdleState.h"
#include "states/HeatingState.h"
#include "states/OverheatErrorState.h"

#include "transitions/TurnOnHeatingTransition.h"
#include "transitions/TurnOffHeatingTransition.h"
#include "transitions/OverHeatingErrorTransition.h"
#include "transitions/TemperatureSensorErrorTransition.h"

class Workflow {
  private:
    std::map<WorkflowState*, std::map<WorkflowTransition*, WorkflowState*>*> nodes;
    WorkflowState *currentState; 
    
    PumpChecker pumpChecker;

    IdleState idleState;
    HeatingState heatingState;
    OverheatErrorState overheatErrorState;

    TurnOnHeatingTransition turnOnHeatingTransition;
    TurnOffHeatingTransition turnOffHeatingTransition;
    OverHeatingErrorTransition overHeatingErrorTransition;
    TemperatureSensorErrorTransition temperatureSensorErrorTransition;

  public:
    Workflow(
      TemperatureSensor *outside, TemperatureSensor *boiler, TemperatureSensor *flue, 
      FlowSensor *flow, 
      Switch *pump, Switch *cooler, Switch *alarm,
      Switch *heater1, Switch *heater2, Switch *heater3,
      ServoController *flueServo, ServoController *boilerServo, 
      HeatingPowerSupplier heatingPower
    ) 
    : turnOnHeatingTransition(outside, flow, pump)
    , turnOffHeatingTransition(outside, heatingPower)
    , pumpChecker(pump, flow)
    , overHeatingErrorTransition(boiler)
    , temperatureSensorErrorTransition(boiler)
    , overheatErrorState(&overHeatingErrorTransition, &temperatureSensorErrorTransition, alarm, pump, cooler, heater1, heater2, heater3, flueServo, boilerServo, &pumpChecker)
    , heatingState(boiler, outside, flue, pump, cooler, heater1, heater2, heater3, flueServo, boilerServo, &pumpChecker)
     {
        currentState = &idleState;
        initTransition(&idleState, &heatingState, &turnOnHeatingTransition);
        initTransition(&heatingState, &idleState, &turnOffHeatingTransition);
        initTransition(&heatingState, &overheatErrorState, &overHeatingErrorTransition);
        initTransition(&heatingState, &overheatErrorState, &temperatureSensorErrorTransition);
        initTransition(&overheatErrorState, &heatingState, &turnOnHeatingTransition);
        initTransition(&overheatErrorState, &idleState, &turnOffHeatingTransition);

        DEBUG_PRINTF("Workflow: initialized with %s state\n", currentState->getName());
    }

    ~Workflow(){
      std::map<WorkflowState*, std::map<WorkflowTransition*, WorkflowState*>*>::iterator it;
      for (it = nodes.begin(); it != nodes.end(); it++){
        delete it->second;
      }
    }

    WorkflowState *getCurrentState(){
      return currentState;
    }

    
    void sync(){
      static unsigned long lastSync = 0;
      if (lastSync == 0 || millis() - lastSync > 3000){
        this->currentState->sync();
        if (this->currentState->canExit()){
          DEBUG_PRINT_LN("Workflow: Checking outgoing transitions");
          std::map<WorkflowTransition*, WorkflowState*> *outgoing = nodes[currentState];
          DEBUG_PRINTF("Workflow: Have %d transitions to check\n", outgoing->size());
          std::map<WorkflowTransition*, WorkflowState*>::iterator it;
          for (it = outgoing->begin(); it != outgoing->end(); it++){
            DEBUG_PRINTF("Workflow: Checking transition %s\n", it->first->getName());
            if (it->first->canHappen()){
              DEBUG_PRINTF("Workflow: about to change state from %s to %s\n", currentState->getName(), it->second->getName());
              currentState->onExit();
              currentState = it->second;
              currentState->onEnter();
              DEBUG_PRINTF("Workflow: finished transition to %s state\n", currentState->getName());
              break;
            }
          }
        }
        else{
          DEBUG_PRINT_LN("Cannot exit current state");
        }
        lastSync = millis();
      }
    }

  private:
    void initTransition(WorkflowState *from, WorkflowState *to, WorkflowTransition *transition){
      std::map<WorkflowTransition*, WorkflowState*> *outgoing = nodes[from];
      if (outgoing == NULL){
        outgoing = new std::map<WorkflowTransition*, WorkflowState*>();
        nodes[from] = outgoing;
      }
      (*outgoing)[transition] = to;
    }
};

#endif //__WORKFLOW__INCLUDED__