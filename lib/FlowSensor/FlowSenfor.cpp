#include "FlowSensor.h"

//This code needs to be outside the class to be placed into RAM code cache
volatile unsigned long ticks; //Number of flow sensor ticks from the start
void ICACHE_RAM_ATTR flowTick()
{
  //This function is called whenever a magnet/interrupt is detected
  ticks++;
} 

FlowSensor::FlowSensor(int interruptPin, Switch *powerSwitch)
{
  ticks = 0;
  attachInterrupt(interruptPin, /*FlowSensor::*/flowTick, RISING);
  powerSwitch->turnOn();
}

void FlowSensor::syncSpeed(){
  unsigned long currentTime = millis();
  if (currentTime - lastSpeedMeasurement >= SPEED_MEASUREMENT_INTERVAL){
    long ticksDelta = ticks - lastTicks;
    long timeDelta = currentTime - lastSpeedMeasurement;
    ticksPerMinute = ticksDelta * 60000 / timeDelta;
    lastTicks = ticks;
    lastSpeedMeasurement = currentTime;
  } 
}

unsigned long FlowSensor::getTicks(){
  return ticks;
}

float FlowSensor::getLitresTotal(){
  return ticks * LITRES_PER_TICK;
}

float FlowSensor::getLitresPerMinute(){
  return ticksPerMinute * LITRES_PER_TICK;
}
