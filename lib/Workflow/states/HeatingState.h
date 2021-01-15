#ifndef __HEATING_STATE__INCLUDED__
#define __HEATING_STATE__INCLUDED__

#include "../WorkflowState.h"

#include <TemperatureSensor.h>
#include <Switch.h>
#include <ServoController.h>

//TODO - place these values to config 
#define MAX_OUTSITE_TEMPERATURE 20.0
#define MIN_OUTSIDE_TEMPERATURE -10.0

#define SECOND_HEATER_ENGAGE_TEMPERATURE 5.0

#define HEATER_MAX_TEMPERATURE  55.0  //Max working temperature for electric heater
#define HEATER_MIN_TEMPERATURE  30.0  //Min working temperature for electric heater
#define HEATER_DELTA_TEMPERATURE 2.5  //Delta around working heater temperature which is allowed

#define FLUE_COOLER_ON  110  //Flue temperature to start cooler to prevent teprerature sensors overheating
#define MAX_ALLOWED_BOILER_TEMPERATURE 75 //Max builder temperature which does not require intervention 

class HeatingState : public WorkflowState
{
  private:
    TemperatureSensor *boiler, *outside, *flue;
    Switch *pump, *cooler;
    Switch *heater1, *heater2, *heater3;
    ServoController *flueServo, *boilerServo;

  private:
    float calculateWorkingTemperature(float outsideTemperature){
      if (outsideTemperature < MIN_OUTSIDE_TEMPERATURE){
        return HEATER_MAX_TEMPERATURE;
      }
      else if (outsideTemperature > MAX_OUTSITE_TEMPERATURE){
        return HEATER_MIN_TEMPERATURE;
      }
      else{
        return HEATER_MIN_TEMPERATURE + (HEATER_MAX_TEMPERATURE - HEATER_MIN_TEMPERATURE) * (MAX_OUTSITE_TEMPERATURE - outsideTemperature) / (MAX_OUTSITE_TEMPERATURE - MIN_OUTSIDE_TEMPERATURE);
      }
    }

    bool isNeedSecondHeater(float boilerTemperature, float workingTemperature, float outsideTemperature){
      return outsideTemperature < SECOND_HEATER_ENGAGE_TEMPERATURE || (workingTemperature - boilerTemperature) > HEATER_DELTA_TEMPERATURE * 2;
    }

    void syncHeater(){
      if (isFireHeatingEngaged()){
        //Fire heating is used, no need for electic heating
        heater1->turnOff(); 
        heater2->turnOff();
      }
      else{
        //Electic heating is used
        float boilerTemperature = boiler->getTemperature();
        float outsideTemperature = outside->getTemperature();
        float workingTemperature = calculateWorkingTemperature(outsideTemperature);
        if (boilerTemperature < workingTemperature - HEATER_DELTA_TEMPERATURE){
          heater1->turnOn(); //TODO - implement check for power needed and engage more then one heater is required
          if (isNeedSecondHeater(boilerTemperature, workingTemperature, outsideTemperature)){
            heater2->turnOn();
          }
        }
        else if (boilerTemperature > workingTemperature + HEATER_DELTA_TEMPERATURE){
          heater1->turnOff(); 
          heater2->turnOff();
        }
      }
    }

    void syncFlueCooler(){
      if (flue->getTemperature() > FLUE_COOLER_ON || boiler->getTemperature() > MAX_ALLOWED_BOILER_TEMPERATURE){
        cooler->turnOn();
      }
      else if (cooler->isOn()) {
        cooler->turnOff();
      }
    }

    bool isFireHeatingEngaged(){
      float boilerTemperature = boiler->getTemperature();
      float outsideTemperature = outside->getTemperature();
      float workingTemperature = calculateWorkingTemperature(outsideTemperature);
      return boiler->getTemperature() > workingTemperature + HEATER_DELTA_TEMPERATURE * 1.5   //Boiler cannot so hot become due to electric heater
          || flue->getTemperature() > boilerTemperature;                                      //Flue is only hot when there is a fire 
    }

    int calculateFlueValveValue(){
      float boilerTemperature = boiler->getTemperature();
      if (boilerTemperature > MAX_ALLOWED_BOILER_TEMPERATURE){
        //Prevent potential overheating by minimizing excaust air flow
        return FLUE_VALVE_CLOSED_VALUE;
      }

      float outsideTemperature = outside->getTemperature();
      float workingTemperature = calculateWorkingTemperature(outsideTemperature);
      float temperature = max(flue->getTemperature(), boilerTemperature + (boilerTemperature - workingTemperature + outsideTemperature) * 2.5F);
      const int maxFlueTemperature = 100;
      const int minFlueTemperature = 20;
      if (temperature < -100){
        return FLUE_VALVE_DEFAULT_VALUE; //Default mode - temperature sensor is not working
      }
      else if (temperature <= minFlueTemperature){
        return FLUE_VALVE_OPEN_VALUE;  
      } 
      else if (temperature >= maxFlueTemperature){
        return FLUE_VALVE_CLOSED_VALUE;  
      }
      else{
        return FLUE_VALVE_OPEN_VALUE + (FLUE_VALVE_CLOSED_VALUE - FLUE_VALVE_OPEN_VALUE) * (temperature - minFlueTemperature) / (maxFlueTemperature - minFlueTemperature); //Linear scale between 20 and 100 celsius (20 = 100%, 100 = 0%)  
      } 
    }

    int calculateBoilerValveValue(){
      //Ensure shutdown air flow in case of being close to overheat
      if (boiler->getTemperature() > MAX_ALLOWED_BOILER_TEMPERATURE){
        return BOILER_VALVE_CLOSED_VALUE;
      }

      //Decide boiler temperature from the outside temperature
      const int maxOutsideTemperature = 5;
      const int minOutsideTemperature = -15;
      float temperature = outside->getTemperature();
      if (temperature < -100){
        return BOILER_VALVE_DEFAULT_VALUE; //Default mode - temperature sensor is not working, so, do not overheat
      }
      else if (temperature >= maxOutsideTemperature){
        return BOILER_VALVE_CLOSED_VALUE;  //Edge case - the warmest weather supported
      }
      else if (temperature <= minOutsideTemperature){
        return BOILER_VALVE_OPEN_VALUE;         //Edge case - the coldest weather supported
      }
      else{
        return BOILER_VALVE_CLOSED_VALUE + (BOILER_VALVE_OPEN_VALUE - BOILER_VALVE_CLOSED_VALUE) * (maxOutsideTemperature - temperature) / (maxOutsideTemperature - minOutsideTemperature); //linear scale between min and max values
      }
    }

    void syncServos(){
      static bool isFireHeatingEngagedBefore = false;
      if (isFireHeatingEngaged()){
        if (!isFireHeatingEngagedBefore){
          //Initiate fire heating - ensure, that servos are in the correct state (they might have moved due to electronic interference) 
          flueServo->setValue(calculateFlueValveValue());
          boilerServo->setValue(calculateBoilerValveValue());
        }
        else{
          flueServo->syncValue(calculateFlueValveValue());
          boilerServo->syncValue(calculateBoilerValveValue());
        }
        isFireHeatingEngagedBefore = true;
      }
      else{
        isFireHeatingEngagedBefore = false;
      }
      flueServo->syncAntiStall();
      boilerServo->syncAntiStall();
    }

  public:
    HeatingState(
      TemperatureSensor *boiler, TemperatureSensor *outside, TemperatureSensor *flue,
      Switch *pump, Switch *cooler, 
      Switch *heater1, Switch *heater2, Switch *heater3, 
      ServoController *flueServo, ServoController *boilerServo
    ){
      this->boiler = boiler;
      this->outside = outside;
      this->flue = flue;
      this->pump = pump;
      this->cooler = cooler;
      this->heater1 = heater1;
      this->heater2 = heater2;
      this->heater3 = heater3;
      this->flueServo = flueServo;
      this->boilerServo = boilerServo;
    }

    const char* getName(){
      return "Heating";
    }

    void printStatus(JsonObject &stateJsonNode){
      float outsideTemperature = outside->getTemperature();
      float workingTemperature = calculateWorkingTemperature(outsideTemperature);
      stateJsonNode["electricHeatingWorkingTemperature"] = workingTemperature;
      stateJsonNode["electricMaxHeatersEngaged"] = isNeedSecondHeater(boiler->getTemperature(), workingTemperature, outsideTemperature) ? 2 : 1;
      stateJsonNode["fireHeatingEngaged"] = isFireHeatingEngaged() ? "on" : "off";
    }

    bool canEnter(){
      return true;
    }
    void onEnter(){
      pump->turnOn();
      flueServo->setValue(FLUE_VALVE_DEFAULT_VALUE);
      boilerServo->setValue(BOILER_VALVE_DEFAULT_VALUE);
    }
    
    void sync(){
      syncHeater();
      syncFlueCooler();
      syncServos();
    }
    
    bool canExit(){
      return true; //Override in case there will be operation which cannot be cancelled.
    }

    void onExit(){
      pump->turnOff();
      heater1->turnOff();
      heater2->turnOff();
      heater3->turnOff();
      cooler->turnOff();
    }
};

#endif //__PENDING_STATE__INCLUDED__