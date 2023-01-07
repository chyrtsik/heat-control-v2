#ifndef __SWITCHES_CONFIG__INCLUDED__
#define __SWITCHES_CONFIG__INCLUDED__

#include <EspDebug.h>
#include <EEPROM.h>

class HeatingConfig
{
private:
    bool isLoaded = false;
    bool isChanged = false; // Set to true when config is changed but not saved

    struct ValveConfig
    {
        int pin;                //Pin number of a bus A which sends signals to servo (0-based)
        int syncInterval;       //Milliseconds between repeating signal to servo to ensure, that it remains in the desired position
        int antiStallInterval;  //Milliseconds between anti-stall operations (moving server to min / max position to ensure, that it does not stuck)
        int activeTime;         //Time in milliseconds to power the servo (should be enough to rotate)
        int openValue;          //Value when valve is fully open
        int closedValue;        //Value when flue valse is fully closed
        int defaultValue;       //Default value for valve when heating (used when dynamic position cannot be calculated)
    };

    struct HeatingStateConfig{
        float maxOutsideTemperature;  //Max outside temperature for heating parameters calibration.
        float minOutsideTemperature;  //Min outside temperature for heating parameters calibration.
        float secondHeaterEngageTemperature; //Temperature below which second electric heater is engaged.

        float electricHeatingMaxTemperature;  //Max working temperature for electric heater
        float electricHeatingMinTemperature;  //Min working temperature for electric heater
        float electricHeatingDeltaTemperature; //Delta around working heater temperature which is allowed

        float flueCoolerOnTemperature;  //Flue temperature to start cooler to prevent teprerature sensors overheating
        float maxAllowedBoilerTemperature; //Max builder temperature which does not require intervention 
    };

    struct IdleStateConfig{
        int timeBetweenFlowChecks;  //Interval between attemps to check water flow in milliseconds 
        int idleFlowCheckDuration;  //Time in milliseconds to turn on pump to check if it is healthy
        float pumpHealthyFlowThreshold; //Minimum flow which is considered healthy for a pump. Decreased flow could indicate pump misfunction.
    };

    struct HeatingTurnOnConfig{
        int timeBetweenFlowChecks; //Interval in milliseconds to verify water flow when conditions for heating are met
        int flowCheckDuration;     //Duration of flow check in milliseconds (should be long enough to allow reliable measurements)
        float flowThreshold;       //Value above this (litres per minute) triggers heating enabling (some of heating devices are open for heating) 
        float maxOutsideTemperature; //Max outside temperature to start checking the need for heating.
        float maxInsideTemperature;  //Max inside temperature to start checking the need for heating. Do not allow room to become colder than desired.
        float outsideTemperatureToForceHeating; //Min temperature to start heating unconditionally
    };

    struct HeatingTurnOffConfig{
        float heatingPowerThreshold; //Min power consumed by the house to turn off heating
        float minOutsideTemperature; //Min temperature to turn off heating
        float maxOutsideTemperature; //Temperature to stop heating unconditionally
    };

    struct ErrorsConfig{
        float boilerOverheatingThreshold; //Min temperature above which boiler is considered overheated
        float pumpFailureFlowThreshold; //Min flow which is valid for pump
        int pumpFailureMinTime;         //Min time in milliseconds pump should be active to check for its falure
        int pumpFailureExpiration;      //Min time in milliseconds to expire error (so that short pump turn on / off do not disable this error)   
    };

    struct ConfigData
    {
        long configVersion;         //Just a magic number to separate junk from real config.
        unsigned int configSize;    //Size of config structure stored in ROM. It can be used to detect when config structure was changed without changing the version.
        
        bool vacationMode;          //True when heating is working in low-temperature mode, and false in case in normal one.
        ValveConfig flueValve;      
        ValveConfig boilerValve;
        HeatingStateConfig heatingState;
        IdleStateConfig idleState;
        HeatingTurnOnConfig heatingTurnOn;
        HeatingTurnOffConfig heatingTurnOff;
        ErrorsConfig errors;
    } configData = {};

    // Default values for configuration (used for new empty hardware)
    #define CONFIG_VERSION 100000000001L       
    #define CONFIG_SIZE    sizeof(ConfigData)

public:
    HeatingConfig()
    {
        load();
    }

    // unsigned int getAutoTurnOnServerDelay()
    // {
    //     return configData.autoTurnOnServerDelay;
    // }

    // void setAutoTurnOnServerDelay(int delay)
    // {
    //     if (delay != configData.autoTurnOnServerDelay)
    //     {
    //         configData.autoTurnOnServerDelay = delay;
    //         isChanged = true;
    //     }
    // }

    void save()
    {
        if (isChanged)
        {
            DEBUG_PRINT_LN("Saving config to EEPROM...");
            EEPROM.begin(sizeof(ConfigData));
            EEPROM.put(0, configData);
            EEPROM.end();
            isChanged = false;
        }
    }

    void load()
    {
        DEBUG_PRINT_LN("Loading config from EEPROM...");
        EEPROM.begin(sizeof(ConfigData));
        EEPROM.get(0, configData);
        if (configData.configVersion != CONFIG_VERSION || configData.configSize != CONFIG_SIZE)
        {
            initDefaultConfig();
        }
        isChanged = false;
    }

private:
    void initDefaultConfig()
    {
        DEBUG_PRINT_LN("Initializing default config...");
        configData.configVersion = CONFIG_VERSION;
        configData.configSize = CONFIG_SIZE;
        configData.vacationMode = false;
    }
} config;

#endif //__SWITCHES_CONFIG__INCLUDED__