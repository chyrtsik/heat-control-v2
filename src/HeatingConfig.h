#ifndef __SWITCHES_CONFIG__INCLUDED__
#define __SWITCHES_CONFIG__INCLUDED__

#include <EspDebug.h>
#include <EEPROM.h>

class HeatingConfig {

public:
  const static int MAX_SWITCH_NAME_LENGTH = 50;

private:
  bool isLoaded = false;

  struct ConfigData {
    long configVersion;
    
  } configData = {};
  
  //Default values for configuration (used for new empty hardware)
  const long CONFIG_VERSION = 100000000001; //Just a magic number to separate junk from real config.

  //TODO - other default config setting go here
  
public:
  HeatingConfig(){
    load();
  }

void load() {
    DEBUG_PRINT_LN("Loading config from EEPROM...");
    EEPROM.begin(sizeof(ConfigData));
    EEPROM.get(0, configData);
    if (configData.configVersion != CONFIG_VERSION) {
      initDefaultConfig();
    }
  }

  void save(){
    DEBUG_PRINT_LN("Saving config to EEPROM...");
    EEPROM.begin(sizeof(ConfigData));
    EEPROM.put(0, configData);
    EEPROM.end();
  }

private:
  void initDefaultConfig() {
    DEBUG_PRINT_LN("Initializing default config...");
    configData.configVersion = CONFIG_VERSION;
    //TODO: initialize other default configs
  }
};

#endif //__SWITCHES_CONFIG__INCLUDED__