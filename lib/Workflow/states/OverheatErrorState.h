#ifndef __OVERHEAT_ERROR_STATE__INCLUDED__
#define __OVERHEAT_ERROR_STATE__INCLUDED__

#include "../WorkflowState.h"
#include "../common/PumpChecker.h"

#include <Switch.h>
#include <ServoController.h>
    
class OverheatErrorState : public WorkflowState
{
  private:
    //TODO - make this an array
    WorkflowTransition *errorTrigger1; //Transition which caused this state. It defines if error state can be existed
    WorkflowTransition *errorTrigger2; //Transition which caused this state. It defines if error state can be existed

    Switch *alarm, *pump, *cooler;
    Switch *heater1, *heater2, *heater3;
    ServoController *flueServo, *boilerServo;
    PumpChecker *pumpChecker;

    bool originalPumpValue, originalCoolerValue;
    int originalFlueServoValue, originalBoilerServoValue;

  public:
    OverheatErrorState(
      WorkflowTransition *errorTrigger1, WorkflowTransition *errorTrigger2, 
      Switch *alarm, Switch *pump, Switch *cooler, 
      Switch *heater1, Switch *heater2, Switch *heater3, 
      ServoController *flueServo, ServoController *boilerServo, 
      PumpChecker *pumpChecker
    ){
      this->errorTrigger1 = errorTrigger1;
      this->errorTrigger2 = errorTrigger2;
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
      flueServo->setValue(80);     //TODO - make configurable. This is a copy-paste of servo value calculation based on temperature
      
      originalBoilerServoValue = boilerServo->getValue();
      boilerServo->setValue(20); //TODO - make configurable. This is a copy-paste of servo value calculation based on temperature
    }
    
    void sync(){
      pumpChecker->check();
    }
    
    bool canExit(){
      return !(errorTrigger1->canHappen() || errorTrigger2->canHappen() || pumpChecker->isBusy());
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

#endif //__OVERHEAT_ERROR_STATE__INCLUDED__