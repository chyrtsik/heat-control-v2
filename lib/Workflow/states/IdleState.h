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

    }

    bool canEnter(){
      return true;
    }

    void onEnter(){
      flueServo->setValue(FLUE_VALSE_CLOSED_VALUE);
      boilerServo->setValue(BOILER_VALCE_CLOSED_VALUE);
    }
    
    void sync(){
      //TODO - check pump periodically (once a month)
      //TODO - check heaters periodically (once a 1-2 months)
      //TODO - perform servos anti-stall (once per week)
    }
    
    bool canExit(){
      //TODO - return false when it will be a process of checking something
      return true;
    }
    void onExit(){
    }
};

#endif //__IDLE_STATE__INCLUDED__