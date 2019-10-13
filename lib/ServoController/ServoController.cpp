#include "ServoController.h"
#include <EspDebug.h>

ServoController::ServoController(const char *servoName, unsigned long servoPin, unsigned long syncInterval, unsigned long antiStallInterval, unsigned long activeTime, ServoValueSupplier getValueFn){
  this->servoName = servoName;
  this->servoPin = servoPin;
  this->syncInterval = syncInterval;
  this->antiStallInterval = antiStallInterval;
  this->activeTime = activeTime;
  this->getValueFn = getValueFn;
}

void ServoController::sync(){
  syncValue();
  syncAntiStall();
}

const char *ServoController::getName(){
  return servoName;  
}

const int ServoController::getValue(){
  return currentValue;  
}

void ServoController::syncAntiStall(){
  if (isTimeForSync(lastAntiStallSyncTime,  antiStallInterval)){
    DEBUG_PRINTF("Starting anti-stall procedure for servo: %s\n", getName());
    sendServoValue(100); 
    sendServoValue(0);
    sendServoValue(currentValue);
    DEBUG_PRINTF("Finished anti-stall procedure for servo: %s\n", getName());
    lastAntiStallSyncTime = millis();
  }
}    

void ServoController::syncValue(){
  if (isTimeForSync(lastServoSyncTime, syncInterval)){
    DEBUG_PRINTF("Recalculating value for servo: %s\n", getName());
    int newValue = getValueFn();
    if (abs(newValue - currentValue) >= 5){
      //Move servo only in case of significant value changes - make less noises and save servo's live.
      currentValue = newValue;
      sendServoValue(currentValue);
    }
    lastServoSyncTime = millis(); 
  }
}

boolean ServoController::isTimeForSync(unsigned long lastSyncTime, unsigned long syncInterval){
  return lastSyncTime == 0 || abs(millis() - lastSyncTime) >= syncInterval;
}

void ServoController::sendServoValue(int percent){
  int value = percent * 160 / 100 + 0; //Servos are working between 0 and 160 degrees angle with this board
  DEBUG_PRINTF("Sending servo %s percent %d, mapped to value %d\n", getName(), percent, value);
  servo.attach(servoPin);
  servo.write(value);
  delay(activeTime);
  servo.detach();
}