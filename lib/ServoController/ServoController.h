#ifndef __SERVO_CONTROLLER__INCLUDED__
#define __SERVO_CONTROLLER__INCLUDED__

#include <BusServo.h>

typedef int (*ServoValueSupplier)();

class ServoController {
  private:
    int servoPin;           //Bit number on sub B (there servos are located)
    unsigned long syncInterval;       //Time between servo state syncs, milliseconds
    unsigned long antiStallInterval;  //Time between anti stall procedure (movement between min and max positions), milliseconds
    unsigned long activeTime;         //Time to pulse servo's pin, milliseconds. Should be short in order not to overload power supply.
    const char *servoName;  
    Servo servo;
    ServoValueSupplier getValueFn;

    int currentValue = 0; 
    unsigned long lastServoSyncTime = 0;
    unsigned long lastAntiStallSyncTime = 0;
  
  public:
    ServoController(const char *servoName, unsigned long servoPin, unsigned long syncInterval, unsigned long antiStallInterval, unsigned long activeTime, ServoValueSupplier getValueFn);
  
    void sync();

    const char *getName();

    const int getValue();
    //TODO - add here mapping from the percent to the actual value

  private:
    void syncAntiStall();
  
    void syncValue();

    boolean isTimeForSync(unsigned long lastSyncTime, unsigned long syncInterval);
  
    void sendServoValue(int percent);
};

#endif //__SERVO_CONTROLLER__INCLUDED__