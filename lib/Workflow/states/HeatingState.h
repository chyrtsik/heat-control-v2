#ifndef __HEATING_STATE__INCLUDED__
#define __HEATING_STATE__INCLUDED__

#include "../WorkflowState.h"
#include "../common/PumpChecker.h"

//TODO - place these values to config 
#define MAX_OUTSITE_TEMPERATURE 20.0
#define MIN_OUTSIDE_TEMPERATURE -20.0

#define SECOND_HEATER_ENGAGE_TEMPERATURE -10.0

#define HEATER_MAX_TEMPERATURE  55.0  //Max working temperature for electric heater
#define HEATER_MIN_TEMPERATURE  35.0  //Min working temperature for electric heater
#define HEATER_DELTA_TEMPERATURE 2.5  //Delta around working heater temperature which is allowed

#define FLUE_COOLER_ON  110  //Flue temperature to start cooler to prevent teprerature sensors overheating

class HeatingState : public WorkflowState
{
  private:
    TemperatureSensor *boiler, *outside, *flue;
    Switch *pump, *cooler;
    Switch *heater1, *heater2, *heater3;
    ServoController *flueServo, *boilerServo;
    PumpChecker *pumpChecker;

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
      float boilerTemperature = boiler->getTemperature();
      float outsideTemperature = outside->getTemperature();
      float workingTemperature = calculateWorkingTemperature(outsideTemperature);
      if (boilerTemperature < workingTemperature - HEATER_DELTA_TEMPERATURE){
        heater2->turnOn(); //TODO - implement check for power needed and engage more then one heater is required
        if (isNeedSecondHeater(boilerTemperature, workingTemperature, outsideTemperature)){
          heater3->turnOn();
        }
      }
      else if (boilerTemperature > workingTemperature + HEATER_DELTA_TEMPERATURE){
        heater2->turnOff(); 
        heater3->turnOff();
      }
    }

    void syncFlueCooler(){
      if (flue->getTemperature() > FLUE_COOLER_ON){
        cooler->turnOn();
      }
      else if (cooler->isOn()) {
        cooler->turnOff();
      }
    }

  public:
    HeatingState(
      TemperatureSensor *boiler, TemperatureSensor *outside, TemperatureSensor *flue,
      Switch *pump, Switch *cooler, 
      Switch *heater1, Switch *heater2, Switch *heater3, 
      ServoController *flueServo, ServoController *boilerServo, 
      PumpChecker *pumpChecker
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
      this->pumpChecker = pumpChecker;
    }

    const char* getName(){
      return "Heating";
    }

    void printStatus(JsonObject &stateJsonNode){
      float outsideTemperature = outside->getTemperature();
      float workingTemperature = calculateWorkingTemperature(outsideTemperature);
      stateJsonNode["workingTemperature"] = workingTemperature;
      stateJsonNode["engagedHeaters"] = isNeedSecondHeater(boiler->getTemperature(), workingTemperature, outsideTemperature) ? 2 : 1;
    }

    bool canEnter(){
      return true;
    }
    void onEnter(){
      pump->turnOn();
    }
    
    void sync(){
      pumpChecker->check();
      flueServo->sync();
      boilerServo->sync();
      syncHeater();
      syncFlueCooler();
    }
    
    bool canExit(){
      return !pumpChecker->isBusy();
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