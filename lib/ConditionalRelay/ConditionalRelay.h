#ifndef __CONDITIONAL_RELAY__INCLUDED__
#define __CONDITIONAL_RELAY__INCLUDED__

#include <Switch.h> 

typedef bool (*ConditionSupplier)(bool isOn);

class ConditionalRelay{
  private:
    ConditionSupplier conditionFn;
    Switch *relay;
    
  public:
    ConditionalRelay(Switch *relay, ConditionSupplier conditionFn);
    void sync();
    boolean isOn();
    const char *getName();
};

#endif //__CONDITIONAL_RELAY__INCLUDED__