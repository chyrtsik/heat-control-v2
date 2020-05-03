#ifndef __HEATING_STATE__INCLUDED__
#define __HEATING_STATE__INCLUDED__

#include "../WorkflowState.h"

class HeatingState : public WorkflowState
{
  public:
    bool canEnter();
    void onEnter();
    
    void sync();
    
    bool canExit();
    void onExit();
};

#endif //__PENDING_STATE__INCLUDED__