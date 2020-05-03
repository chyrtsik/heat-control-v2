#include "PendingState.h"
#include <EspDebug.h>

const char* PendingState::getName(){
  return "Pending";
}

bool PendingState::canEnter(){
  //TODO - calculate, if heating can be turned off
  return true;
}

void PendingState::onEnter(){
  //TODO
  DEBUG_PRINT_LN("On enter Pending"); 
}

void PendingState::sync(){
  //TODO
  DEBUG_PRINT_LN("Sync Pending");
}

bool PendingState::canExit(){
  //TODO - return false in case there is an action in progress
  return true;
}

void PendingState::onExit(){
  //TODO
  DEBUG_PRINT_LN("On exit Pending");
}