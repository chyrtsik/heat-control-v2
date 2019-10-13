#include "TermoRelay.h"

TermoRelay::TermoRelay(float onTemp, float offTemp, Switch *relay, Switch *ledAlarm, boolean stateOnError){
  this->relay = relay;
  this->ledAlarm = ledAlarm;
  this->onTemp = onTemp;
  this->offTemp = offTemp;
  this->stateOnError = stateOnError;
}  

TermoRelay::TermoRelay(float onTemp, float offTemp, Switch *relay, Switch *ledAlarm) : TermoRelay(onTemp, offTemp, relay, ledAlarm,false) {}

boolean TermoRelay::isOn(){
  return relay->isOn();
}

const char *TermoRelay::getName(){
  return relay->getName();
}

float TermoRelay::getOnTemp(){
  return onTemp;
}

float TermoRelay::getOffTemp(){
  return offTemp;
}

void TermoRelay::processTemp(float currentTemp){
  if (isValidWaterTemp(currentTemp)){
    ledAlarm->turnOff();
    if ((onTemp < offTemp && currentTemp <= onTemp) || (onTemp > offTemp && currentTemp >= onTemp)){
      if (!isOn()){
        relay->turnOn();
      }
    }
    else if((onTemp < offTemp && currentTemp >= offTemp) || (onTemp > offTemp && currentTemp <= offTemp)){
      if (isOn()){
        relay->turnOff();
      }
    }
  }
  else{
    //Temperature detector seems to be broken
    ledAlarm->turnOn();
    if (stateOnError){
      relay->turnOn();
    }
    else {
      relay->turnOff();
    }
  }
}

boolean TermoRelay::isValidWaterTemp(float temperature){
  return temperature > 0; 
}  