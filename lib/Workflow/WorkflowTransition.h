#ifndef __WORKFLOW_TRANSITION__INCLUDED__
#define __WORKFLOW_TRANSITION__INCLUDED__

class WorkflowTransition
{
  public:
    virtual const char* getName();
    virtual bool canHappen();
};

#endif //__WORKFLOW_TRANSITION__INCLUDED__