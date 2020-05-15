#ifndef __ERROR_STATE__INCLUDED__
#define __ERROR_STATE__INCLUDED__

#include "../WorkflowState.h"
#include "../common/PumpChecker.h"

#include <list>
#include <Switch.h>
#include <ServoController.h>
    
class ErrorState : public WorkflowState
{
  private:
    typedef std::list<WorkflowTransition*> TriggersList;
    TriggersList triggers; //Transitions, which can cause this error
    
    Switch *alarm, *pump, *cooler;
    Switch *heater1, *heater2, *heater3;
    ServoController *flueServo, *boilerServo;
    PumpChecker *pumpChecker;

    bool originalPumpValue, originalCoolerValue;
    int originalFlueServoValue, originalBoilerServoValue;

    bool hasActiveTriggers(){
      for (TriggersList::iterator it = triggers.begin(); it != triggers.end(); it++){
        if ((*it)->canHappen()){
          return true;
        }
      }
      return false;
    }

  public:
    ErrorState(
      TriggersList triggers, 
      Switch *alarm, Switch *pump, Switch *cooler, 
      Switch *heater1, Switch *heater2, Switch *heater3, 
      ServoController *flueServo, ServoController *boilerServo, 
      PumpChecker *pumpChecker
    ){
      this->triggers = triggers;
      this->alarm = alarm;
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
      return "Error"; 
    }

    void printStatus(JsonObject &stateJsonNode){
      JsonArray triggersJson = stateJsonNode.createNestedArray("activeErrors");
      for (TriggersList::iterator it = triggers.begin(); it != triggers.end(); it++){
        if ((*it)->canHappen()){
          triggersJson.add((*it)->getName());
        }
      }
    }

    bool canEnter(){
      return true;
    }

    void onEnter(){
      alarm->turnOn();
      
      originalPumpValue = pump->isOn();
      pump->turnOn();

      originalCoolerValue = cooler->isOn();
      cooler->turnOn();

      heater1->turnOff();
      heater2->turnOff();
      heater3->turnOff();

      originalFlueServoValue = flueServo->getValue();
      flueServo->setValue(FLUE_VALSE_CLOSED_VALUE);
      
      originalBoilerServoValue = boilerServo->getValue();
      boilerServo->setValue(BOILER_VALCE_CLOSED_VALUE);
    }
    
    void sync(){
      pumpChecker->check();
    }
    
    bool canExit(){
      return !(pumpChecker->isBusy() || hasActiveTriggers());
    }

    void onExit(){
      alarm->turnOff();
      if (!originalPumpValue) {
        pump->turnOff();
      }
      if (!originalCoolerValue){
        cooler->turnOff();
      }
      flueServo->setValue(originalFlueServoValue);
      boilerServo->setValue(originalBoilerServoValue);
    }
};

#endif //__ERROR_STATE__INCLUDED__