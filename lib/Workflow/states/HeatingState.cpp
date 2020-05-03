#include "HeatingState.h"
#include <EspDebug.h>

const char* HeatingState::getName(){
  return "Heating";
}

bool HeatingState::canEnter(){
  //TODO - calculate, if heating can be turned off
  return true;
}

void HeatingState::onEnter(){
  //TODO
  DEBUG_PRINT_LN("On enter Heating"); 
}

void HeatingState::sync(){
  //TODO
  DEBUG_PRINT_LN("Sync Heating");
}

bool HeatingState::canExit(){
  //TODO - return false in case there is an action in progress
  return true;
}

void HeatingState::onExit(){
  //TODO
  DEBUG_PRINT_LN("On exit Heating");
}