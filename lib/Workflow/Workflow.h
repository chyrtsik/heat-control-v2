#ifndef __WORKFLOW__INCLUDED__
#define __WORKFLOW__INCLUDED__

#include <map>
#include <EspDebug.h>
#include <ArduinoJson.h>

#include "WorkflowState.h"
#include "WorkflowTransition.h"

#include "states/IdleState.h"
#include "states/HeatingState.h"
#include "states/errorState.h"

#include "transitions/TurnOnHeatingTransition.h"
#include "transitions/TurnOffHeatingTransition.h"
#include "transitions/errors/OverHeatingErrorTransition.h"
#include "transitions/errors/TemperatureSensorErrorTransition.h"
#include "transitions/errors/UnexpectedHeatingErrorTransition.h"
#include "transitions/errors/PumpFailureErrorTransition.h"

#define WORKFLOW_SYNC_DELAY 1000

class Workflow {
  private:
    std::map<WorkflowState*, std::map<WorkflowTransition*, WorkflowState*>*> nodes;
    WorkflowState *currentState; 
    
    IdleState idleState;
    HeatingState heatingState;
    ErrorState errorState;

    WorkflowState *states[3];
    
    OverHeatingErrorTransition overHeatingErrorTransition;
    TemperatureSensorErrorTransition temperatureSensorErrorTransition;
    UnexpectedHeatingErrorTransition unexpectedHeatingErrorTransition;
    PumpFailureErrorTransition pumpFailureErrorTransition;
    TurnOnHeatingTransition turnOnHeatingTransition;
    TurnOffHeatingTransition turnOffHeatingTransition;

  public:
    Workflow(
      TemperatureSensor *outside, TemperatureSensor *inside, TemperatureSensor *boiler, TemperatureSensor *flue, 
      FlowSensor *flow, 
      Switch *pump, Switch *cooler,
      Switch *heater1, Switch *heater2, Switch *heater3,
      ServoController *flueServo, ServoController *boilerServo, 
      HeatingPowerSensor *heatingPower
    ) 
    : turnOnHeatingTransition(outside, inside, flow, pump)
    , turnOffHeatingTransition(outside, flow, heatingPower, &turnOnHeatingTransition)
    , overHeatingErrorTransition(boiler)
    , temperatureSensorErrorTransition(boiler)
    , unexpectedHeatingErrorTransition(boiler, flue, &turnOnHeatingTransition)
    , pumpFailureErrorTransition(pump, flow)
    , idleState(flueServo, boilerServo, pump, flow)
    , heatingState(boiler, outside, flue, pump, cooler, heater1, heater2, heater3, flueServo, boilerServo)
    , errorState( 
        {&overHeatingErrorTransition, &temperatureSensorErrorTransition,&unexpectedHeatingErrorTransition, &pumpFailureErrorTransition}, 
        pump, cooler, heater1, heater2, heater3, flueServo, boilerServo
      )
     {
        currentState = &idleState;
        states[0] = &idleState;
        states[1] = &heatingState;
        states[2] = &errorState;
        
        initTransition(&idleState, &errorState, &temperatureSensorErrorTransition);
        initTransition(&idleState, &errorState, &unexpectedHeatingErrorTransition);
        initTransition(&idleState, &errorState, &pumpFailureErrorTransition);
        initTransition(&idleState, &heatingState, &turnOnHeatingTransition);
        
        initTransition(&heatingState, &idleState, &turnOffHeatingTransition);
        initTransition(&heatingState, &errorState, &overHeatingErrorTransition);
        initTransition(&heatingState, &errorState, &temperatureSensorErrorTransition);
        initTransition(&heatingState, &errorState, &pumpFailureErrorTransition);
        
        initTransition(&errorState, &heatingState, &turnOnHeatingTransition);
        initTransition(&errorState, &idleState, &turnOffHeatingTransition);
    }

    ~Workflow(){
      std::map<WorkflowState*, std::map<WorkflowTransition*, WorkflowState*>*>::iterator it;
      for (it = nodes.begin(); it != nodes.end(); it++){
        delete it->second;
      }
    }

    /**
     * Attempt moving workflow to the given state. Returns true in case it is successful.
     */
    bool activateState(const char* stateName){
      WorkflowState *newState = findStateByName(stateName);
      if (newState != NULL){
          if (currentState->canExit() && newState->canEnter()){
            DEBUG_PRINTF("Workflow: about to override state from %s to %s\n", currentState->getName(), newState->getName());
            currentState->onExit();
            currentState = newState;
            currentState->onEnter();
            DEBUG_PRINTF("Workflow: finished override to %s state\n", currentState->getName());
          }
      }
      return false;
    }

    /**
     * Dump current worflow status to the given json
     */
    void printStatus(JsonObject &workflowJsonNode){
      JsonObject stateJsonNode = workflowJsonNode.createNestedObject("currentState");
      stateJsonNode["name"] = currentState->getName();
      currentState->printStatus(stateJsonNode);
    }

    
    void sync(){
      static unsigned long lastSync = 0;
      if (lastSync == 0){
        currentState->onEnter();
        DEBUG_PRINTF("Workflow: initialized with %s state\n", currentState->getName());
      }

      if (lastSync == 0 || millis() - lastSync > WORKFLOW_SYNC_DELAY){
        this->currentState->sync();
        if (this->currentState->canExit()){
          std::map<WorkflowTransition*, WorkflowState*> *outgoing = nodes[currentState];
          std::map<WorkflowTransition*, WorkflowState*>::iterator it;
          for (it = outgoing->begin(); it != outgoing->end(); it++){
            if (it->first->canHappen() && it->second->canEnter()){
              DEBUG_PRINTF("Workflow: about to change state from %s to %s\n", currentState->getName(), it->second->getName());
              currentState->onExit();
              currentState = it->second;
              currentState->onEnter();
              DEBUG_PRINTF("Workflow: finished transition to %s state\n", currentState->getName());
              break;
            }
          }
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

    WorkflowState* findStateByName(const char *stateName){
      for (unsigned int i=0; i<sizeof(states) / sizeof(states[0]); i++){
        if (strcmp(states[i]->getName(), stateName) == 0){
          return states[i];
        }
      }
    }
};

#endif //__WORKFLOW__INCLUDED__