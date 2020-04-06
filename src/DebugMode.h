/*
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
}*/


void setPins(int pins[], int totalPins, bool value){
  for (int i=0; i<totalPins; i++){
    DEBUG_PRINTF("Setting pin %d to %s\n", pins[i], value ? "high" : "low");
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], value ? HIGH : LOW);
  }
}

//Verify PIOs workng correctly to send out signals
void debugPIOs(){
  int pins[] = ALL_DIGITAL_PINS;
  int totalPins = sizeof(pins) / sizeof(pins[0]);
  DEBUG_PRINTF("Total pins: %d\n", totalPins);
  setPins(pins, totalPins, true);
  delay(30000);
  setPins(pins, totalPins, false);
  delay(30000);
}

// Verify that bits are received correctly by each bit on the parallel port
void debugPorts(){
  pinMode(PIN_SET, OUTPUT);
  pinMode(PIN_CLK, OUTPUT);
  pinMode(PIN_DATA, OUTPUT);
  delay(500); //Give bus time to reset (this can be tuned)

  ParallelBus *buses[] = {&busA, &busB};
  for (int bus = 0; bus < sizeof(buses) / sizeof(buses[0]); bus++){
    for(int i=0; i<8; i++){
      buses[bus]->setBit(i, true);
    }
  }
  delay(10000);
  for (int bus = 0; bus < sizeof(buses) / sizeof(buses[0]); bus++){
    for(int i=0; i<8; i++){
      buses[bus]->setBit(i, false);
    }
  }
  delay(10000);
}

void debugLoop(){
  //Debug switches on and off
  //debugPIOs();
  //debugPorts();
  //debugSwitches(); 
}

void debugSetup(){
  DEBUG_INIT();
  WiFi.mode(WIFI_OFF);
}