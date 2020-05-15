#ifndef __WORKFLOW_STATE__INCLUDED__
#define __WORKFLOW_STATE__INCLUDED__

#include <ArduinoJson.h>

class WorkflowState
{
  public:
    virtual const char* getName();
    virtual void printStatus(JsonObject &stateJsonNode);
    
    virtual bool canEnter();
    virtual void onEnter();
    
    virtual void sync();
    
    virtual bool canExit();
    virtual void onExit();
};

#endif //__WORKFLOW_STATE__INCLUDED__