#ifndef __ERROR_STATE__INCLUDED__
#define __ERROR_STATE__INCLUDED__

#include "../WorkflowState.h"
#include "../WorkflowTransition.h"

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

    bool originalPumpValue, originalCoolerValue;
    int originalFlueServoValue, originalBoilerServoValue;

    unsigned long lastEnabledErrorMode = 0;

    bool hasActiveTriggers(){
      for (TriggersList::iterator it = triggers.begin(); it != triggers.end(); it++){
        if ((*it)->canHappen()){
          return true;
        }
      }
      return false;
    }

    void enableErrorMode(){
      pump->turnOn();
      cooler->turnOn();
      heater1->turnOff();
      heater2->turnOff();
      heater3->turnOff();
      flueServo->setValue(FLUE_VALVE_CLOSED_VALUE);
      
      //Ensure to close the door as tight as possible by flaping it
      boilerServo->setValue(BOILER_VALVE_OPEN_VALUE);
      boilerServo->setValue(BOILER_VALVE_CLOSED_VALUE);
      boilerServo->setValue(BOILER_VALVE_OPEN_VALUE);
      boilerServo->setValue(BOILER_VALVE_CLOSED_VALUE);
      
      lastEnabledErrorMode = millis();
    }

    void reEnableErrorMode(){
      if (millis() - lastEnabledErrorMode > 60000){
        //Ensure to have error mode enabled (so that custom override is not doable for long time)
        enableErrorMode();
      }
      else{ 
        //Just ensure that heating is not enabled
        heater1->turnOff();
        heater2->turnOff();
        heater3->turnOff();
      }
    }

  public:
    ErrorState(
      TriggersList triggers, 
      Switch *pump, Switch *cooler, 
      Switch *heater1, Switch *heater2, Switch *heater3, 
      ServoController *flueServo, ServoController *boilerServo
    ){
      this->triggers = triggers;
      this->pump = pump;
      this->cooler = cooler;
      this->heater1 = heater1;
      this->heater2 = heater2;
      this->heater3 = heater3;
      this->flueServo = flueServo;
      this->boilerServo = boilerServo;
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
      originalPumpValue = pump->isOn();
      originalCoolerValue = cooler->isOn();
      originalFlueServoValue = flueServo->getValue();
      originalBoilerServoValue = boilerServo->getValue();
      enableErrorMode();
    }
    
    void sync(){
      reEnableErrorMode();
    }
    
    bool canExit(){
      return !hasActiveTriggers();
    }

    void onExit(){
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