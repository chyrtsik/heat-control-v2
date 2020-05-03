#ifndef __ERROR_STATE__INCLUDED__
#define __ERROR_STATE__INCLUDED__

#include "../WorkflowState.h"

class ErrorState : public WorkflowState
{
  public:
    bool canEnter();
    void onEnter();
    
    void sync();
    
    bool canExit();
    void onExit();
};

#endif //__ERROR_STATE__INCLUDED__