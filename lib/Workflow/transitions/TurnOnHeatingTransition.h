#ifndef __TURN_ON_HEATING_TRANSITION__INCLUDED__
#define __TURN_ON_HEATING_TRANSITION__INCLUDED__

#include "../WorkflowTransition.h"
#include <TemperatureSensor.h>
#include <FlowSensor.h>
#include <Switch.h>

class TurnOnHeatingTransition : public WorkflowTransition
{
  public:
    TemperatureSensor *outside;
    FlowSensor *flow;
    Switch *pump;

    TurnOnHeatingTransition(TemperatureSensor *outside, FlowSensor *flow, Switch *pump){
      this->outside = outside;
      this->flow = flow;
      this->pump = pump;
    }

  private:
    bool isTooCold(){
      return outside->getTemperature() < 0; //TODO - make this configurable
    }

    bool isColdENoughToTryFlow(){
      return outside->getTemperature() < 15; //TODO - make this configurable
    }

    bool isMeasuring = false;
    float measuredFlow = 0.0;
    unsigned long lastMeasurement = 0;
    unsigned long measurementStart = 0;
    const unsigned long TIME_BETWEEN_MEASUREMENTS = 3600000; //TODO - make this configurable
    const unsigned long MEASUREMENT_DURATION = 30000;        //TODO - make this configurable 
    bool hasEnoughWaterFlow(){
      unsigned long currentTime = millis();
      if (isMeasuring){
        if (currentTime - measurementStart > MEASUREMENT_DURATION){
          //Stop measurements
          measuredFlow = flow->getLitresPerMinute();
          lastMeasurement = currentTime;
          isMeasuring = false;
          pump->turnOff();
        }    
      }
      else {
        if (lastMeasurement == 0 || currentTime - lastMeasurement > TIME_BETWEEN_MEASUREMENTS){
          if (!pump->isOn()){
            //Start measurements
            isMeasuring = true;
            measurementStart = currentTime;
            pump->turnOn();
          }
          else if (pump->getTimeInLastState() > TIME_BETWEEN_MEASUREMENTS){
            //Pump is already running. We can use flow measurements right away.
            measuredFlow = flow->getLitresPerMinute();
            lastMeasurement = currentTime;
          }
        }
      }
      
      return measuredFlow > 10.0; //TODO - make this configurable
    }

  public:
    const char* getName(){
      return "TurnOnHeating";
    }

    bool canHappen(){
      return isTooCold() || (isColdENoughToTryFlow() ? hasEnoughWaterFlow() : false);
    }
};

#endif //__TURN_ON_HEATING_TRANSITION__INCLUDED__