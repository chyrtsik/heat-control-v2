#ifndef __WORKFLOW_STATE__INCLUDED__
#define __WORKFLOW_STATE__INCLUDED__

class WorkflowState
{
  public:
    virtual bool canEnter();
    virtual void onEnter();
    
    virtual void sync();
    
    virtual bool canExit();
    virtual void onExit();
};

#endif //__WORKFLOW_STATE__INCLUDED__