#include "ParallelBus.h"
#include "../../include/Settings.h"
#include <Arduino.h>
#include <EspDebug.h>

void ICACHE_RAM_ATTR _bus_pulsePin(int pin){
  digitalWrite(pin, HIGH);
  digitalWrite(pin, LOW);
}

void ICACHE_RAM_ATTR _bus_sendBits(const int addr[], const int bits[]){
  for (int i=0; i<3; i++){
    digitalWrite(PIN_DATA, addr[i] ? HIGH : LOW);
    _bus_pulsePin(PIN_CLK);
  }
  for (int i=0; i<8; i++){
    digitalWrite(PIN_DATA, bits[i] ? HIGH : LOW);
    _bus_pulsePin(PIN_CLK);
  }
  _bus_pulsePin(PIN_SET);
}

void ICACHE_RAM_ATTR _bus_sync(const int addr[], const int bits[]){
  noInterrupts();
  _bus_sendBits(addr, bits);    
  interrupts();
}