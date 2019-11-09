#include "ConditionalRelay.h"

ConditionalRelay::ConditionalRelay(Switch *relay, ConditionSupplier conditionFn){
  this->relay = relay;
  this->conditionFn = conditionFn;
}

boolean ConditionalRelay::isOn(){
  return relay->isOn();
}

const char *ConditionalRelay::getName(){
  return relay->getName();
}

void ConditionalRelay::sync(){
  bool currentlyOn = isOn();
  if (conditionFn(currentlyOn)){
    if (!currentlyOn){
      relay->turnOn();
    }
  }
  else{
    if (currentlyOn){
      relay->turnOff();
    }
  }
}