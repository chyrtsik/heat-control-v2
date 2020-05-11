#ifndef __IDLE_STATE__INCLUDED__
#define __IDLE_STATE__INCLUDED__

#include "../WorkflowState.h"

class IdleState : public WorkflowState
{
  public:
    const char* getName(){ 
      return "Idle"; 
    }

    bool canEnter(){
      return true;
    }

    void onEnter(){
    }
    
    void sync(){
      //TODO - check pump periodically (once a month)
      //TODO - check heaters periodically (once a 1-2 months)
      //TODO - perform servos anti-stall (once per week)
    }
    
    bool canExit(){
      //TODO - return false when it will be a process of checking something
      return true;
    }
    void onExit(){
    }
};

#endif //__IDLE_STATE__INCLUDED__