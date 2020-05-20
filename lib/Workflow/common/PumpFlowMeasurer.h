#ifndef __PUMP_FLOW_MEASURER__INCLUDED__
#define __PUMP_FLOW_MEASURER__INCLUDED__

#include <Switch.h>
#include <FlowSensor.h>

class PumpFlowMeasurer {
  private:
    Switch *pump;
    FlowSensor *flow;
    unsigned long timeBetweenMeasurements, measurementDuration;
    
    bool isMeasuring = false;
    float measuredFlow = 0.0;
    unsigned long lastMeasurement = 0;
    unsigned long measurementStart = 0;

  public:
    PumpFlowMeasurer(Switch *pump, FlowSensor *flow, unsigned long timeBetweenMeasurements, unsigned long measurementDuration){
      this->pump = pump;
      this->flow = flow;
      this->timeBetweenMeasurements = timeBetweenMeasurements;
      this->measurementDuration = measurementDuration;
    }

    float getLastMeasuredFlow(){
      unsigned long currentTime = millis();
      if (isMeasuring){
        if (currentTime - measurementStart > measurementDuration){
          //Stop measurements
          measuredFlow = flow->getLitresPerMinute();
          lastMeasurement = currentTime;
          isMeasuring = false;
          pump->turnOff();
        }    
      }
      else {
        if (lastMeasurement == 0 || currentTime - lastMeasurement > timeBetweenMeasurements){
          if (!pump->isOn()){
            //Start measurements
            isMeasuring = true;
            measurementStart = currentTime;
            pump->turnOn();
          }
          else if (pump->getTimeInLastState() > timeBetweenMeasurements){
            //Pump is already running. We can use flow measurements right away.
            measuredFlow = flow->getLitresPerMinute();
            lastMeasurement = currentTime;
          }
        }
      }
      
      return measuredFlow;
    }

    bool isBusy(){
      return isMeasuring;
    }
};

#endif //__PUMP_FLOW_MEASURER__INCLUDED__