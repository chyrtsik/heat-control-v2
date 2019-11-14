////////////////////////////////
// Requests  hanglers
const int BUFFER_SIZE = 1024;

void sendResponse(const char *contentType, const char *content) {
  server.sendHeader("Connection", "close");
  server.send(200, contentType, content);
}

void sendJsonResponse(JsonDocument& jsonDoc) {
  char responseBuff[(int)(BUFFER_SIZE * 1.2)]; //20% extra for pretty responses.
  serializeJsonPretty(jsonDoc, responseBuff, sizeof(responseBuff));
  sendResponse("text/json", responseBuff);
}

void sendTextResponse(const char * text){
  sendResponse("text/plain", text);
}

void sendHtmlResponse(const char * html){
  sendResponse("text/html", html);
}

void sendBadRequestResponse(const char *content) {
  server.sendHeader("Connection", "close");
  server.send(400, "application/json", content);
}

String calculateVersion(){
  return String(__DATE__) + " " + __TIME__;
}

void handleGetRoot() {
  busy();
  
  DEBUG_PRINT_LN("Processing GET /");

  //Monitoring
  String response = "<style>#monitoring{width: 100%;} .value{font-size: xx-large;} </style>";
  response += "<table id=\"monitoring\"><tbody><tr><td class=\"value\">";
  response += boilerTemp.getTemperature();
  response += "&deg;C</td><td class=\"value\">";
  response += calculateCurrentPower();
  response += "KW</td><td class=\"value\">";
  response += flowSensor.getLitresPerMinute();
  response += "L/min</td><td class=\"value\">";
  response += insideTemp.getTemperature();
  response += "&deg;C</td><td class=\"value\">";
  response += outsideTemp.getTemperature();
  response += "&deg;C</td>";
  response += "</tr><tr><td>boiler</td><td>power</td><td>flow</td><td>inside</td><td>outside</td></tr></tbody></table>";

  //Heating control
  response += "</p>";
  response += "<form method='POST' action='/status?switches.heater2=on&switches.heater3=on'><input type='submit' value='Electric heating ON' style='width: 100%;height: 20%;font-size: xx-large;'></form>";
  response += "<form method='POST' action='/status?switches.heater2=off&switches.heater3=off'><input type='submit' value='Electric heating OFF' style='width: 100%;height: 20%;font-size: xx-large;'></form>";

  //Version of other info  
  response += "<p>Build date and time: " + calculateVersion() + "</p>";
  // unsigned long currentTime = millis();
  // int seconds = (currentTime / 1000) % 60;
  // int minutes = (currentTime / (60 * 1000)) % 60;
  // int hours = (currentTime / (60 * 60 * 1000)) % 24;
  // int days = currentTime / (24 * 60 * 60 * 1000);
  // response += "<p>Uptime: ";
  // response += days + " days ";
  // response += hours + " hours ";
  // response += minutes + " minutes ";
  // response += seconds + " seconds </p>";

  // response += "<p>Endpoints available: <ul><li>GET/PUT /api/status</li><li>GET|PUT /api/config</li><li>GET /api/sensors</li></ul></p>";


  sendHtmlResponse(response.c_str());

  notBusy();
}

void handleApiGetStatus() {
  busy();
  
  DEBUG_PRINT_LN("Processing GET /status");

  StaticJsonDocument<BUFFER_SIZE> root;
  
  root["version"] = calculateVersion();
  root["uptime"] = millis();
  
  JsonObject sensorsJson = root.createNestedObject("sensors");
  for(int i=0; i<tempSensorsCount; i++){
    sensorsJson[tempSensors[i]->getName()] = tempSensors[i]->getTemperature();
  }

  JsonObject switchesJson = root.createNestedObject("switches");
  for(int i=0; i<switchesCount; i++){
    switchesJson[switches[i]->getName()] = switches[i]->isOn () ? "on" : "off";
  }

  JsonObject flowSensorJson = root.createNestedObject("flowSensor");
  flowSensorJson["ticks"] = flowSensor.getTicks(); 
  flowSensorJson["litresTotal"] = flowSensor.getLitresTotal(); 
  flowSensorJson["litresPerMinute"] = flowSensor.getLitresPerMinute();
  
  JsonObject powerJson = root.createNestedObject("power");
  powerJson["heating"] = calculateCurrentPower(); 

  JsonObject valvesJson = root.createNestedObject("valves");
  for(int i=0; i<valvesCount; i++) {
    valvesJson[valves[i]->getName()] = valves[i]->getValue();
  }

  sendJsonResponse(root);
  notBusy();
}

void handleApiGetSensors() {
  busy();
  
  DEBUG_PRINT_LN("Processing GET /sensors");

  StaticJsonDocument<BUFFER_SIZE> root;
  
  int numberOfDevices = sensors.getDeviceCount();
  root["count"] = numberOfDevices;
  root["parasite"] = sensors.isParasitePowerMode() ? "on" : "off";
  root["conversion"] = sensors.getWaitForConversion() ? "wait" : "async";

  JsonArray devices = root.createNestedArray("devices");
  sensors.begin(); //start sensors in case this is the very first call
  for(int i=0;i<numberOfDevices; i++) {
    JsonObject device = devices.createNestedObject();
    DeviceAddress deviceAddress;
    if(sensors.getAddress(deviceAddress, i)) {
      JsonArray address = device.createNestedArray("address");
      for (int i = 0; i < 8; i++) address.add(deviceAddress[i]);
    } 
  }
  
  sendJsonResponse(root);

  notBusy();
}

bool applyNewStatus(){
  for (int i = 0; i < server.args(); i++){
    String argName = server.argName(i);
    if (argName.startsWith("switches.")){
      //Turn on / off switch by name
      String switchName = argName.substring(9);
      for (int s = 0; s < switchesCount; s++){
        if (switchName.equals(switches[s]->getName())){
          String switchValue = server.arg(i);
          if (switchValue.equals("on")){
            DEBUG_PRINTF("Turning on switch %s\n", switchName.c_str());
            switches[s]->turnOn();  
          }
          else if (switchValue.equals("off")){
            DEBUG_PRINTF("Turning off switch %s\n", switchName.c_str());
            switches[s]->turnOff();
          }
        }
      }
    }
    else if (argName.startsWith("valves.")){
      //Change valve value by name
      String valveName = argName.substring(7);
      for (int v = 0; v < valvesCount; v++){
        if (valveName.equals(valves[v]->getName())){
          String valveValue = server.arg(i);
          int intValue = valveValue.toInt();
          if (intValue >= 0 && intValue <= 100){
            DEBUG_PRINTF("Setting valve %s to to value %d\n", valveName.c_str(), intValue);
            valves[v]->setValue(intValue);  
          }
        }
      }
    }
  }
  return true;
}

void handleApiPutStatus() {
  busy();
  DEBUG_PRINT_LN("Processing PUT /status");
  if (applyNewStatus()){
    sendTextResponse("OK");
  }
  else{
    sendBadRequestResponse("Ignored. Check the arguments.");
  }
  notBusy();
}

void handlePostStatus(){
  busy();
  DEBUG_PRINT_LN("Processing POST /status");

  if (applyNewStatus()){
    sendHtmlResponse("<html><head><meta http-equiv='refresh' content='2;url=/'/></head><body>OK</body></html>");
  }
  else{
    sendBadRequestResponse("Ignored. Check the request.");
  }
  
  notBusy();
}

void handleApiGetConfig() {
  busy();
  DEBUG_PRINT_LN("Processing GET /config");
  
  StaticJsonDocument<BUFFER_SIZE> root;
  
  JsonArray relays = root.createNestedArray("termoRelays");
  for (int i = 0; i < termoRelaysCount; i++) {
    JsonObject relayConfig = relays.createNestedObject();
    TermoRelay *relay = termoRelays[i];
    relayConfig["name"] = (char*)relay->getName();
    relayConfig["onTemp"] = relay->getOnTemp();
    relayConfig["offTemp"] = relay->getOffTemp();
  }
  
  sendJsonResponse(root);
  notBusy();
}
  
void handleApiPutConfig() {
  busy();
  DEBUG_PRINT_LN("Processing PUT /config");

  //TODO - implement

  sendBadRequestResponse("Ignored. Check the request.");

  notBusy();
}


void handleGetUpdate(){
  busy();
  DEBUG_PRINT_LN("Processing GET /update");
  sendHtmlResponse("<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>");
  notBusy();
}

void handlePostUpdateResult(){
  busy();
  DEBUG_PRINT_LN("Processing POST /update (result)");
  sendTextResponse((Update.hasError())?"FAIL":"OK");
  yield();
  notBusy();
  ESP.restart();
}

void handlePostUpdate(){
  busy();
  HTTPUpload& upload = server.upload();
  if(upload.status == UPLOAD_FILE_START){
    WiFiUDP::stopAll();
    DEBUG_PRINTF("Update: %s\n", upload.filename.c_str());
    uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
    Update.begin(maxSketchSpace);
  } else if(upload.status == UPLOAD_FILE_WRITE){
    Update.write(upload.buf, upload.currentSize);
  } else if(upload.status == UPLOAD_FILE_END){
    if(Update.end(true)){ //true to set the size to the current progress
      DEBUG_PRINTF("Update Success: %u\nRebooting...\n", upload.totalSize);
    }
  }
  yield();
  notBusy();
}

void handleGetReboot(){
  busy();
  DEBUG_PRINT_LN("Processing GET /reboot");
  sendHtmlResponse("<form method='POST' action='/reboot'><input type='submit' value='Reboot the heating control'></form>");
  notBusy();
}

void handlePostReboot(){
  busy();
  DEBUG_PRINT_LN("Processing POST /reboot");
  sendTextResponse("Rebooting...");
  yield();
  notBusy();
  ESP.restart();
}