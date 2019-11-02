#ifndef __PALALLEL_BUS_INCLUDED___
#define __PALALLEL_BUS_INCLUDED___

class ParallelBus{
  private:
    const int *addr; 
    int state[8] = {0,0,0,0,0,0,0,0};

  public: 
    ParallelBus(const int *addr);

    void sync();

    void setBit(int bitNo, bool isOn);

    bool getBit(int bitNo);
};

#endif //__PALALLEL_BUS_INCLUDED___