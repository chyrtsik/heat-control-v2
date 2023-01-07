#ifndef __SWITCHES_CONFIG__INCLUDED__
#define __SWITCHES_CONFIG__INCLUDED__

#include <EspDebug.h>
#include <EEPROM.h>

class HeatingConfig {
private:
  bool isLoaded = false;
  bool isChanged = false; //Set to true when config is changed but not saved

  struct ConfigData {
    //TODO - put config data here
    long configVersion;
    bool autoTurnOnServer;
    int autoTurnOnServerDelay;
  } configData = {};
  
  //Default values for configuration (used for new empty hardware)
  const int DEFAULT_AUTO_TURN_ON_SERVER = false;
  const int DEFAULT_AUTO_TURN_ON_SERVER_DELAY = 600000; //Turn on server in 10 minutes after it being off
  const long CONFIG_VERSION = (long)100000000003; //Just a magic number to separate junk from real config.

public:
  HeatingConfig(){
    load();
  }

  bool isAutoTurnOnServer() {
    return configData.autoTurnOnServer;
  }

  void setAutoTurnOnServer(bool isEnabled){
    if (isEnabled != configData.autoTurnOnServer){
        configData.autoTurnOnServer = isEnabled;
        isChanged = true;
    }
  }

  unsigned int getAutoTurnOnServerDelay() {
    return configData.autoTurnOnServerDelay;
  }

  void setAutoTurnOnServerDelay(int delay){
    if (delay != configData.autoTurnOnServerDelay){
        configData.autoTurnOnServerDelay = delay;
        isChanged = true;
    }
  }

  void save(){
    if (isChanged){
        DEBUG_PRINT_LN("Saving config to EEPROM...");
        EEPROM.begin(sizeof(ConfigData));
        EEPROM.put(0, configData);
        EEPROM.end();
        isChanged = false;
    }
  }

  void load() {
    DEBUG_PRINT_LN("Loading config from EEPROM...");
    EEPROM.begin(sizeof(ConfigData));
    EEPROM.get(0, configData);
    if (configData.configVersion != CONFIG_VERSION) {
      initDefaultConfig();
    }
    isChanged = false;
  }

private:
  void initDefaultConfig() {
    DEBUG_PRINT_LN("Initializing default config...");
    configData.configVersion = CONFIG_VERSION;
    configData.autoTurnOnServer = DEFAULT_AUTO_TURN_ON_SERVER;
    configData.autoTurnOnServerDelay = DEFAULT_AUTO_TURN_ON_SERVER_DELAY;
  }
};

#endif //__SWITCHES_CONFIG__INCLUDED__