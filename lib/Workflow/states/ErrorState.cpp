#include "ErrorState.h"
#include <EspDebug.h>

const char* ErrorState::getName(){
  return "Error";
}

bool ErrorState::canEnter(){
  //TODO - calculate, if heating can be turned off
  return true;
}

void ErrorState::onEnter(){
  //TODO
  DEBUG_PRINT_LN("On enter Error"); 
}

void ErrorState::sync(){
  //TODO
  DEBUG_PRINT_LN("Sync Error");
}

bool ErrorState::canExit(){
  //TODO - return false in case there is an action in progress
  return true;
}

void ErrorState::onExit(){
  //TODO
  DEBUG_PRINT_LN("On exit Error");
}