#include "ParallelBus.h"
#include "../../include/Settings.h"
#include <Arduino.h>
#include <EspDebug.h>

void ICACHE_RAM_ATTR _bus_pulsePin(int pin){
  digitalWrite(pin, HIGH);
  digitalWrite(pin, LOW);
}

void ICACHE_RAM_ATTR _bus_sendBits(const int bits[]){
  for (int i=7; i>=0; i--){
    digitalWrite(PIN_DATA, bits[i] ? HIGH : LOW);
    _bus_pulsePin(PIN_CLK);
  }
  _bus_pulsePin(PIN_SET);
}

void ICACHE_RAM_ATTR _bus_sync(const int bits[]){
  noInterrupts();
  _bus_sendBits(bits);    
  interrupts();
}

ParallelBus::ParallelBus(){
  sync(); //initialize on creation
}

void ICACHE_RAM_ATTR ParallelBus::sync(){
  _bus_sync(state);
}

void ICACHE_RAM_ATTR ParallelBus::setBit(int bitNo, bool isOn){
  this->state[bitNo] = isOn ? 1 : 0;
  _bus_sync(state);
}

bool ICACHE_RAM_ATTR ParallelBus::getBit(int bitNo){
  return this->state[bitNo] == 1 ? true : false;  
}