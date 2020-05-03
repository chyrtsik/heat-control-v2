#ifndef __WORKFLOW__INCLUDED__
#define __WORKFLOW__INCLUDED__

#include <map>
#include <list>
#include <EspDebug.h>

#include "WorkflowState.h"
#include "states/PendingState.h"
#include "states/HeatingState.h"
#include "states/ErrorState.h"

class Workflow {
  private:
    std::map<WorkflowState*, std::list<WorkflowState*>*> states;
    WorkflowState *currentState; 
    
    PendingState pendingState;
    HeatingState heatingState;
    ErrorState errorState;

  public:
    Workflow(){
       //TODO - initialize workflow to initial state
       currentState = &pendingState;
       initTransition(&pendingState, &heatingState, &errorState);
       initTransition(&heatingState, &errorState, &pendingState);
       initTransition(&errorState, &pendingState, &heatingState);
    }

    ~Workflow(){
      std::map<WorkflowState*, std::list<WorkflowState*>*>::iterator it;
      for (it = states.begin(); it != states.end(); it++){
        delete it->second;
      }
    }

    void sync(){
      this->currentState->sync();
      if (this->currentState->canExit()){
        std::list<WorkflowState*> *nextStates = states.at(currentState);
        for (std::list<WorkflowState*>::iterator nextState = nextStates->begin(); nextState != nextStates->end(); nextState++){
          if ((*nextState)->canEnter()){
            currentState->onExit();
            currentState = (*nextState);
            currentState->onEnter();
            break;
          }
        }
      }
    }

  private:
    void initTransition(WorkflowState *from, WorkflowState *to1, WorkflowState *to2){
      std::list<WorkflowState*> *next = new std::list<WorkflowState*>();
      next->push_back(to1);
      next->push_back(to2);
      states.insert(std::make_pair(from, next));
    }
};

#endif //__WORKFLOW__INCLUDED__