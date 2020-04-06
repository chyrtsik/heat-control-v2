#ifndef __PALALLEL_BUS_INCLUDED___
#define __PALALLEL_BUS_INCLUDED___

class ParallelBus{
  private:
    const int *addr; 
    int addrLen;
    int *state;
    int stateLen;

  public: 
    ParallelBus(const int *addr, const int addrLen, const int stateLen);

    ~ParallelBus();

    void sync();

    void setBit(int bitNo, bool isOn);

    bool getBit(int bitNo);
};

#endif //__PALALLEL_BUS_INCLUDED___