#ifndef __IDLE_STATE__INCLUDED__
#define __IDLE_STATE__INCLUDED__

#include "../WorkflowState.h"

#include <ServoController.h>

class IdleState : public WorkflowState
{ 
  private:  
    ServoController *flueServo, *boilerServo;

  public:
    IdleState(ServoController *flueServo, ServoController *boilerServo){
      this->flueServo = flueServo;
      this->boilerServo = boilerServo;
    }

    const char* getName(){ 
      return "Idle"; 
    }

    void printStatus(JsonObject &stateJsonNode){
      //TODO - log here diagnostic info (heaters / pump health). Also, this is possible, that health is to be stored together with them and always logged
    }

    bool canEnter(){
      return true;
    }

    void onEnter(){
      flueServo->setValue(FLUE_VALVE_CLOSED_VALUE);
      boilerServo->setValue(BOILER_VALVE_CLOSED_VALUE);
    }
    
    void sync(){
      //TODO - check pump periodically (once a month)

      //TODO - check heaters periodically (once a 1-2 months)

      //Ensure that valves do not stuck 
      flueServo->syncAntiStall();
      boilerServo->syncAntiStall();
    }
    
    bool canExit(){
      //TODO - return false when it will be a process of checking something
      return true;
    }
    void onExit(){
    }
};

#endif //__IDLE_STATE__INCLUDED__