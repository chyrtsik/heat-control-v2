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

void handleGetRoot() {
  busy();
  
  DEBUG_PRINT_LN("Processing GET /");

  String response = "<p>Build date and time: ";
  response = response + __DATE__ + " " + __TIME__ + "</p>";

  unsigned long currentTime = millis();
  int seconds = (currentTime / 1000) % 60;
  int minutes = (currentTime / (60 * 1000)) % 60;
  int hours = (currentTime / (60 * 60 * 1000)) % 24;
  int days = currentTime / (24 * 60 * 60 * 1000);
  response = response + "<p>Uptime: " + days + " days " + hours + " hours " + minutes + " minutes " + seconds + " seconds" + "</p>";

  response = response + "<p>Endpoints available: <ul><li>GET /api/status</li><li>GET|PUT /api/config</li><li>GET /api/sensors</li></ul></p>";

  sendHtmlResponse(response.c_str());

  notBusy();
}

void handleApiGetStatus() {
  busy();
  
  DEBUG_PRINT_LN("Processing GET /status");

  StaticJsonDocument<BUFFER_SIZE> root;
  
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

void handleApiPutStatus() {
  busy();
  DEBUG_PRINT_LN("Processing PUT /status");
  sendBadRequestResponse("Not supported yet");
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


void handlePostStatus(){
  busy();
  DEBUG_PRINT_LN("Processing POST /status");

  /*if (applyNewStatus()){
    sendHtmlResponse("<html><head><meta http-equiv='refresh' content='2;url=/'/></head><body>OK</body></html>");
  }
  else{*/
    sendBadRequestResponse("Ignored. Check the request.");
  //}
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