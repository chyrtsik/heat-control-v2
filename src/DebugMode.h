void switchesOn(){
  for(int i=0; i<switchesCount; i++){
    switches[i]->turnOn();
  }
}

void switchesOff(){
  for(int i=0; i<switchesCount; i++){
    switches[i]->turnOff();
  }
}

void debugSwitches(){
  delay(3000);
  switchesOn();
  delay(3000);
  switchesOff();
}

void debugLoop(){
  //Debug switches on and off
  debugSwitches();  
}

void debugSetup(){
  //Implement custom init if needed.
}