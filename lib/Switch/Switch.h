#ifndef __SWITCH_INCLUDED__
#define __SWITCH_INCLUDED__

#include <Arduino.h>
#include <ParallelBus.h>

class Switch{
  private:
    ParallelBus *bus;
    int bitNo;
    const char *switchName;
    unsigned long lastStateChange = 0;
    
  public:
    Switch(ParallelBus &bus, int bitNo, const char *switchName);
    boolean isOn();
    const char *getName();
    void turnOn();
    void turnOff();
    unsigned long getTimeInLastState();
};

#endif //__SWITCH_INCLUDED__