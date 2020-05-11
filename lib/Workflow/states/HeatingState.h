#ifndef __HEATING_STATE__INCLUDED__
#define __HEATING_STATE__INCLUDED__

#include "../WorkflowState.h"

class HeatingState : public WorkflowState
{
  public:
    const char* getName(){
      return "Heating";
    }

    bool canEnter(){
      return true;
    }
    void onEnter(){
      //TODO: Enable heating (pump, heater relay, servos when needed)
    }
    
    void sync(){
      //TODO - sync termo relays (and adjust working temperature depending on weather and heating power)
      //TODO - check servos (anti-stall)
      //TODO - check pump health
    }
    
    bool canExit(){
      //TODO - return false, in case there is an ongoing process (see above)
      return true;
    }
    void onExit(){
      //TODO - turn on 
    }
};

#endif //__PENDING_STATE__INCLUDED__