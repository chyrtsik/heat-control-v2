#include "ParallelBus.h"
#include "../../include/Settings.h"
#include <Arduino.h>
#include <EspDebug.h>

void ICACHE_RAM_ATTR _bus_pulsePin(int pin){
  digitalWrite(pin, HIGH);
  digitalWrite(pin, LOW);
}

void ICACHE_RAM_ATTR _bus_sendBits(const int addr[], const int addrLen, const int bits[], const int bitsLen){
  for (int i=0; i<addrLen; i++){
    digitalWrite(PIN_DATA, addr[i] ? HIGH : LOW);
    _bus_pulsePin(PIN_CLK);
  }
  for (int i=0; i<bitsLen; i++){
    digitalWrite(PIN_DATA, bits[i] ? HIGH : LOW);
    _bus_pulsePin(PIN_CLK);
  }
  _bus_pulsePin(PIN_SET);
}

void ICACHE_RAM_ATTR _bus_sync(const int addr[], const int addrLen, const int bits[], const int bitsLen){
  noInterrupts();
  _bus_sendBits(addr, addrLen, bits, bitsLen);    
  interrupts();
}

ParallelBus::ParallelBus(const int *addr, const int addrLen, const int stateLen){
  this->addr = addr;
  this->addrLen = addrLen;
  this->stateLen = stateLen;
  this->state = new int[stateLen];
  memset(state, 0, sizeof(state[0]) * stateLen);
  sync(); //initialize on creation
}

ParallelBus::~ParallelBus(){
  delete this->state;
}

void ICACHE_RAM_ATTR ParallelBus::sync(){
  _bus_sync(addr, addrLen, state, stateLen);
}

void ICACHE_RAM_ATTR ParallelBus::setBit(int bitNo, bool isOn){
  this->state[bitNo] = isOn ? 1 : 0;
  _bus_sync(addr, addrLen, state, stateLen);
}

bool ICACHE_RAM_ATTR ParallelBus::getBit(int bitNo){
  return this->state[bitNo] == 1 ? true : false;  
}