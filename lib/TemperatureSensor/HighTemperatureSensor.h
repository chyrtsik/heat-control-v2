#ifndef __HIGH_TEMPERATURE_SENSOR__INCLUDED__
#define __HIGH_TEMPERATURE_SENSOR__INCLUDED__

#include "TemperatureSensor.h"
#include <Arduino.h>
#include <EspDebug.h>

/**
 * Analog high temperature sensor. Its resistance to temperature: 26C = 110Ohm, 100C = 150 Ohm, 200C = 180Omh, 300 = 210 Omh, 400 = 235 Omh, 
 **/
class HighTemperatureSensor : public TemperatureSensor {
  private:
    int analogSensorPin;

  public:
    HighTemperatureSensor(int analogSensorPin, const char *sensorName) : TemperatureSensor(sensorName){
      this->analogSensorPin = analogSensorPin;
      pinMode(analogSensorPin, INPUT);
    }

  protected:
    float measureTemp()
    {
      int value = analogRead(analogSensorPin);
      float voltage = value / 1024.0;
      float resistance = voltage * 4700 / (5.0 - voltage); //Rt = Ut * Ro / (U - Ut) - resistance from voltage drop
      float temp;
      if (resistance < 50 || resistance >= 235){
        temp = 400.0; //Max temperature measured by sensor. Temperatures above drot its resitance to ~10 Omh. 
      }
      else if (resistance < 150){
        //26С - 100С segment
        temp = -20 /*Imperic correction*/ + 26.0 + (resistance - 110) / (150 - 110) * (100 - 26); //t = to + (R - R0) / (R1 - R0) * (t1 - t0) -- linear approximation by 2 points. 
      }
      else if (resistance < 210){
        //100С - 300С segment
        temp = 100.0 + (resistance - 150) / (210 - 150) * (300 - 100);
      }
      else{
        //300С - 400C segment
        temp = 300.0 + (resistance - 210) / (235 - 210) * (400 - 300);
      }
      DEBUG_PRINTF("High temperature %s: value=%d, voltage=%f, resistance=%f, temp=%f\n", getName(), value, voltage, resistance, temp);
      return round(temp); //Sensor is not high precision - single degrees should be fine
    }
};

#endif //__HIGH_TEMPERATURE_SENSOR__INCLUDED__