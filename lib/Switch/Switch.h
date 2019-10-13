#ifndef __SWITCH_INCLUDED__
#define __SWITCH_INCLUDED__

#include <Arduino.h>
#include <ParallelBus.h>

class Switch{
  private:
    ParallelBus *bus;
    int bitNo;
    const char *switchName;
    
  public:
    Switch(ParallelBus &bus, int bitNo, const char *switchName);
    boolean isOn();
    const char *getName();
    void turnOn();
    void turnOff();
};

#endif //__SWITCH_INCLUDED__