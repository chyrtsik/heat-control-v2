#include "ConsulRegistration.h"
#include <EspDebug.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

ConsulRegistration::ConsulRegistration(const char *serviceName, int refreshInterval){
  this->refreshInterval = refreshInterval;
  this->serviceName = serviceName;
  this->lastRefresh = 0;
}

void ConsulRegistration::refresh(){
  if (lastRefresh == 0 || abs(millis() - lastRefresh) >= refreshInterval){
    lastRefresh = millis();
    registerInConsul();
  } 
}

boolean ConsulRegistration::registerInConsul(){
  String ip = WiFi.localIP().toString();
  DEBUG_PRINT_LN("Updating consul registration...");
  if (submitServerIp(ip.c_str())){
    DEBUG_PRINT_LN("Registered in consul.");
    return true;
  }
  else{
    DEBUG_PRINT_LN("Failed consul registration.");
    return false;
  }
}

boolean ConsulRegistration::submitServerIp(const char *ip){
  const char *payload = createPayload(ip, serviceName);
  DEBUG_PRINTF("Requesting consul endpoint: %s with payload: %s\n", consulEndpoint, payload);
  
  HTTPClient http;
  http.begin(consulEndpoint);
  http.addHeader("Content-Type", "application/json");
  int rc = http.sendRequest("PUT", payload);
  http.end();
  
  return rc == 200;
}

const char *ConsulRegistration::createPayload(const char *ip, const char *serviceName){
  snprintf(requestBuffer, sizeof(requestBuffer), "{\"Address\": \"%s\", \"Node\": \"%s\", \"Service\": {\"Service\": \"%s\", \"Address\": \"%s\", \"Port\": 80}}}", ip, serviceName, serviceName, ip);
  return requestBuffer;
}