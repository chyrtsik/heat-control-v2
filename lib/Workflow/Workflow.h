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

class Workflow {
  private:
    std::map<WorkflowState*, std::map<WorkflowTransition*, WorkflowState*>*> nodes;
    WorkflowState *currentState; 
    
    PumpChecker pumpChecker;

    IdleState idleState;
    HeatingState heatingState;
    ErrorState errorState;

    OverHeatingErrorTransition overHeatingErrorTransition;
    TemperatureSensorErrorTransition temperatureSensorErrorTransition;
    UnexpectedHeatingErrorTransition unexpectedHeatingErrorTransition;
    TurnOnHeatingTransition turnOnHeatingTransition;
    TurnOffHeatingTransition turnOffHeatingTransition;

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
    , unexpectedHeatingErrorTransition(boiler, flue, &turnOnHeatingTransition)
    , idleState(flueServo, boilerServo)
    , errorState( {&overHeatingErrorTransition, &temperatureSensorErrorTransition,&unexpectedHeatingErrorTransition}, alarm, pump, cooler, heater1, heater2, heater3, flueServo, boilerServo, &pumpChecker)
    , heatingState(boiler, outside, flue, pump, cooler, heater1, heater2, heater3, flueServo, boilerServo, &pumpChecker)
     {
        currentState = &idleState;
        
        initTransition(&idleState, &errorState, &temperatureSensorErrorTransition);
        initTransition(&idleState, &errorState, &unexpectedHeatingErrorTransition);
        initTransition(&idleState, &heatingState, &turnOnHeatingTransition);
        
        initTransition(&heatingState, &idleState, &turnOffHeatingTransition);
        initTransition(&heatingState, &errorState, &overHeatingErrorTransition);
        initTransition(&heatingState, &errorState, &temperatureSensorErrorTransition);
        
        initTransition(&errorState, &heatingState, &turnOnHeatingTransition);
        initTransition(&errorState, &idleState, &turnOffHeatingTransition);
    }

    ~Workflow(){
      std::map<WorkflowState*, std::map<WorkflowTransition*, WorkflowState*>*>::iterator it;
      for (it = nodes.begin(); it != nodes.end(); it++){
        delete it->second;
      }
    }

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