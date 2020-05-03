#ifndef __PENDING_STATE__INCLUDED__
#define __PENDING_STATE__INCLUDED__

#include "../WorkflowState.h"

class PendingState : public WorkflowState
{
  public:
    const char* getName();

    bool canEnter();
    void onEnter();
    
    void sync();
    
    bool canExit();
    void onExit();
};

#endif //__PENDING_STATE__INCLUDED__