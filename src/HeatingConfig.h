#ifndef __HEATING_CONFIG__INCLUDED__
#define __HEATING_CONFIG__INCLUDED__

#include <EspDebug.h>
#include <EEPROM.h>

class HeatingConfig
{
private:
    bool isLoaded = false;
    bool isChanged = false; // Set to true when config is changed but not saved

    struct ValveConfig
    {
        int pin;               // Pin number of a bus A which sends signals to servo (0-based)
        int syncInterval;      // Milliseconds between repeating signal to servo to ensure, that it remains in the desired position
        int antiStallInterval; // Milliseconds between anti-stall operations (moving server to min / max position to ensure, that it does not stuck)
        int activeTime;        // Time in milliseconds to power the servo (should be enough to rotate)
        int openValue;         // Value when valve is fully open
        int closedValue;       // Value when flue valse is fully closed
        int defaultValue;      // Default value for valve when heating (used when dynamic position cannot be calculated)
    };

    struct HeatingStateConfig
    {
        float maxOutsideTemperature;         // Max outside temperature for heating parameters calibration.
        float minOutsideTemperature;         // Min outside temperature for heating parameters calibration.
        float secondHeaterEngageTemperature; // Temperature below which second electric heater is engaged.

        float electricHeatingMaxTemperature;   // Max working temperature for electric heater
        float electricHeatingMinTemperature;   // Min working temperature for electric heater
        float electricHeatingDeltaTemperature; // Delta around working heater temperature which is allowed

        float flueCoolerOnTemperature;     // Flue temperature to start cooler to prevent teprerature sensors overheating
        float maxAllowedBoilerTemperature; // Max builder temperature which does not require intervention
    };

    struct IdleStateConfig
    {
        int timeBetweenFlowChecks;      // Interval between attemps to check water flow in milliseconds
        int idleFlowCheckDuration;      // Time in milliseconds to turn on pump to check if it is healthy
        float pumpHealthyFlowThreshold; // Minimum flow which is considered healthy for a pump. Decreased flow could indicate pump misfunction.
    };

    struct HeatingTurnOnConfig
    {
        int timeBetweenFlowChecks;                // Interval in milliseconds to verify water flow when conditions for heating are met
        int flowCheckDuration;                    // Duration of flow check in milliseconds (should be long enough to allow reliable measurements)
        float flowThreshold;                      // Value above this (litres per minute) triggers heating enabling (some of heating devices are open for heating)
        float maxOutsideTemperature;              // Max outside temperature to start checking the need for heating.
        float maxInsideTemperature;               // Max inside temperature to start checking the need for heating. Do not allow room to become colder than desired.
        float outsideTemperatureToForceHeatingOn; // Min temperature to start heating unconditionally
    };

    struct HeatingTurnOffConfig
    {
        float heatingPowerThreshold;               // Min power consumed by the house to turn off heating
        float minOutsideTemperature;               // Min temperature to turn off heating
        float outsideTemperatureToForceHeatingOff; // Temperature to stop heating unconditionally
    };

    struct ErrorsConfig
    {
        float boilerOverheatingThreshold; // Min temperature above which boiler is considered overheated
        float pumpFailureFlowThreshold;   // Min flow which is valid for pump
        int pumpFailureMinTime;           // Min time in milliseconds pump should be active to check for its falure
        int pumpFailureExpiration;        // Min time in milliseconds to expire error (so that short pump turn on / off do not disable this error)
    };

    struct VacationModeConfig
    {
        bool enabled;                          // True when heating is working in low-temperature mode, and false in case in normal one.
        
        float maxOutsideTemperature;           //Override of max outside temperature for heating calculations
        float minOutsideTemperature;           //Override of min outside temperature for heating calculations
        
        float electricHeatingMaxTemperature;   // Override of Max working temperature for electric heater
        float electricHeatingMinTemperature;   // Override of  Min working temperature for electric heater
        float electricHeatingDeltaTemperature; // Override of Delta around working heater temperature which is allowed

        float heatingTurnOnMaxOutsideTemperature; // Override of Max outside temperature to consider heating in vacation mode
        float heatingTurnOnMaxInsideTemperature;  // Override of Max inside temperature to consider heating in vacation mode

        float heatingTurnOffMinOutsideTemperature;               // Override of Min temperature to turn off heating
        float heatingTurnOffOutsideTemperatureToForceHeatingOff; // Override of Temperature to stop heating unconditionally
    };

    struct ConfigData
    {
        long configVersion;         // Just a magic number to separate junk from real config.
        unsigned int configSize;    // Size of config structure stored in ROM. It can be used to detect when config structure was changed without changing the version.
        ValveConfig flueValve;
        ValveConfig boilerValve;
        HeatingStateConfig heatingState;
        IdleStateConfig idleState;
        HeatingTurnOnConfig heatingTurnOn;
        HeatingTurnOffConfig heatingTurnOff;
        ErrorsConfig errors;
        VacationModeConfig vacationMode;
    };

    // Default values for configuration (used for new empty hardware)
    static const long CONFIG_VERSION = (long)1000000002;
    static const unsigned int CONFIG_SIZE = sizeof(ConfigData);

public:
    ConfigData configData = {};

    HeatingConfig()
    {
        load();
    }

    bool isVacationModeEnabled()
    {
        return configData.vacationMode.enabled;
    }

    void setVacationModeEnabled(bool isEnabled)
    {
        if (isEnabled != configData.vacationMode.enabled)
        {
            configData.vacationMode.enabled = isEnabled;
            makeChanged();
        }
    }

    void makeChanged(){
        isChanged = true;
    }

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
            save();
        }
        isChanged = false;
    }

private:
    void initDefaultConfig()
    {
        DEBUG_PRINT_LN("Initializing default config...");
        configData.configVersion = CONFIG_VERSION;
        configData.configSize = CONFIG_SIZE;

        configData.flueValve.pin = 6;                      // A7
        configData.flueValve.syncInterval = 20000;         // Each 20 seconds (needs to be interactive to control overheating)
        configData.flueValve.antiStallInterval = 86400000; // Each 24 hours = 24 * 60 * 60 * 1000 ms
        configData.flueValve.activeTime = 1000;            // 1 second is enough to move flue valve, as it is light
        configData.flueValve.openValue = 0;
        configData.flueValve.closedValue = 100;
        configData.flueValve.defaultValue = 0; // Open by default as we expect fire to be started first

        configData.boilerValve.pin = 7;                       // A8
        configData.boilerValve.syncInterval = 600000;         // Each 10 minutes = 10 * 60 * 60 * 1000 ms
        configData.boilerValve.antiStallInterval = 604800000; // Each 7 days = 7 * 24 * 60 * 60 * 1000 ms
        configData.boilerValve.activeTime = 1000;             // 1 second is enough to move flue valve, as it is light
        configData.boilerValve.openValue = 0;
        configData.boilerValve.closedValue = 100;
        configData.boilerValve.defaultValue = 50; // Middle temperature be default as we expect fire to be started first

        configData.heatingState.maxOutsideTemperature = 20.0;
        configData.heatingState.minOutsideTemperature = -10.0;
        configData.heatingState.secondHeaterEngageTemperature = 5.0;
        configData.heatingState.electricHeatingMaxTemperature = 55.0;
        configData.heatingState.electricHeatingMinTemperature = 30.0;
        configData.heatingState.electricHeatingDeltaTemperature = 2.5;
        configData.heatingState.flueCoolerOnTemperature = 110.0;
        configData.heatingState.maxAllowedBoilerTemperature = 75.0;

        configData.idleState.timeBetweenFlowChecks = 604800000; // Each 7 days = 14 * 24 * 60 * 60 * 1000 ms
        configData.idleState.idleFlowCheckDuration = 60000;     // 1 minute = 1 * 60 * 1000 ms
        configData.idleState.pumpHealthyFlowThreshold = 8.0;    // Should be less than max bypass capacity to prevent false alerts

        configData.heatingTurnOn.timeBetweenFlowChecks = 7200000; // Check pump each 2 hours
        configData.heatingTurnOn.flowCheckDuration = 30000;       // Measure flow for 30 seconds for reliable readings
        configData.heatingTurnOn.flowThreshold = 12.0;
        configData.heatingTurnOn.maxOutsideTemperature = 18.0;
        configData.heatingTurnOn.maxInsideTemperature = 24.0;
        configData.heatingTurnOn.outsideTemperatureToForceHeatingOn = 1.0;

        configData.heatingTurnOff.heatingPowerThreshold = 0.5; // 0.5 KW
        configData.heatingTurnOff.minOutsideTemperature = 8.0;
        configData.heatingTurnOff.outsideTemperatureToForceHeatingOff = 20.0;

        configData.errors.boilerOverheatingThreshold = 85.0;
        configData.errors.pumpFailureFlowThreshold = 1.0;
        configData.errors.pumpFailureMinTime = 10000;
        configData.errors.pumpFailureExpiration = 50000;

        configData.vacationMode.enabled = false;
        configData.vacationMode.maxOutsideTemperature = 10.0;
        configData.vacationMode.minOutsideTemperature = -10.0;
        configData.vacationMode.electricHeatingMaxTemperature = 40.0;
        configData.vacationMode.electricHeatingMinTemperature = 20.0;
        configData.vacationMode.electricHeatingDeltaTemperature = 3.0;
        configData.vacationMode.heatingTurnOnMaxOutsideTemperature = 6.0;
        configData.vacationMode.heatingTurnOnMaxInsideTemperature = 8.0;
        configData.vacationMode.heatingTurnOffMinOutsideTemperature = 4.0;
        configData.vacationMode.heatingTurnOffOutsideTemperatureToForceHeatingOff = 10.0;
    }
} config;

#endif //__HEATING_CONFIG__INCLUDED__