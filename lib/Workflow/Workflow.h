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

  public:
    Workflow(
      TemperatureSensor *outside, TemperatureSensor *boiler, 
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
    , overheatErrorState(&overHeatingErrorTransition, alarm, pump, cooler, heater1, heater2, heater3, flueServo, boilerServo, &pumpChecker) 
     {
        //TODO - initialize workflow to initial state
        currentState = &idleState;
        initTransition(&idleState, &heatingState, &turnOnHeatingTransition);
        initTransition(&heatingState, &idleState, &turnOffHeatingTransition);
        initTransition(&heatingState, &overheatErrorState, &overHeatingErrorTransition);
        initTransition(&overheatErrorState, &heatingState, &turnOnHeatingTransition);
        initTransition(&overheatErrorState, &idleState, &turnOffHeatingTransition);
    }

    ~Workflow(){
      std::map<WorkflowState*, std::map<WorkflowTransition*, WorkflowState*>*>::iterator it;
      for (it = nodes.begin(); it != nodes.end(); it++){
        delete it->second;
      }
    }

    void sync(){
      if (this->currentState->canExit()){
        std::map<WorkflowTransition*, WorkflowState*> *outgoing = nodes[currentState];
        std::map<WorkflowTransition*, WorkflowState*>::iterator it;
        for (it = outgoing->begin(); it != outgoing->end(); it++){
          if (it->first->canHappen()){
            currentState->onExit();
            currentState = it->second;
            currentState->onEnter();
            break;
          }
        }
      }
    }

  private:
    void initTransition(WorkflowState *from, WorkflowState *to, WorkflowTransition *transition){
      std::map<WorkflowTransition*, WorkflowState*> *outgoing = nodes[from];
      if (outgoing == NULL){
        outgoing = new std::map<WorkflowTransition*, WorkflowState*>();
      }
      (*outgoing)[transition] = to;
    }
};

#endif //__WORKFLOW__INCLUDED__