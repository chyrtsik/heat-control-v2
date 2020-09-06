#include "Switch.h"

Switch::Switch(ParallelBus &bus, int bitNo, const char *switchName){
  this->bus = &bus;
  this->bitNo = bitNo;
  this->switchName = switchName;
  turnOff(); //off by default
}

boolean Switch::isOn(){
  return bus->getBit(bitNo);
}

const char *Switch::getName(){
  return switchName;
}

void Switch::turnOn(){
  bus->setBit(bitNo, true);
  lastStateChange = millis();
}

void Switch::turnOff(){
  bus->setBit(bitNo, false);
  lastStateChange = millis();
}

unsigned long Switch::getTimeInLastState(){
  return millis() - lastStateChange;
}