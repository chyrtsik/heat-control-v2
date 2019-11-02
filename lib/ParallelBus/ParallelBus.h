#ifndef __PALALLEL_BUS_INCLUDED___
#define __PALALLEL_BUS_INCLUDED___

void _bus_pulsePin(int pin);
void _bus_sendBits(const int addr[], const int bits[]);
void _bus_sync(const int addr[], const int bits[]);

class ParallelBus{
  private:
    const int *addr; 
    int state[8] = {0,0,0,0,0,0,0,0};

  public: 
    ParallelBus(const int *addr){
      this->addr = addr;
      sync(); //initialize on creation
    }

    void sync(){
      _bus_sync(addr, state);
    }

    void setBit(int bitNo, bool isOn){
      this->state[bitNo] = isOn ? 1 : 0;
      _bus_sync(addr, state);
    }

    bool getBit(int bitNo){
      return this->state[bitNo] == 1 ? true : false;  
    }
};

#endif //__PALALLEL_BUS_INCLUDED___