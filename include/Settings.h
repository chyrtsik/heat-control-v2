#ifndef __SETTING_H__INCLUDED__
#define __SETTING_H__INCLUDED__

////////////////////////////////////////////////////////////////////////////
// Tunable settings (to be tuned and changes for each board)

// Timings
#define BUS_SYNC_DELAY     1000       //Delay between resyncing state of the parallel bus
#define TERMO_SYNC_DELAY   2000      //Delay between temperaature sensor readings

//Valves configuration (controlled by servos)
#define FLUE_VALVE_PIN  config.configData.flueValve.pin
#define FLUE_VALVE_SYNC_INTERVAL config.configData.flueValve.syncInterval
#define FLUE_VALVE_ANTI_STALL_INTERVAL config.configData.flueValve.antiStallInterval
#define FLUE_VALVE_ACTIVE_TIME config.configData.flueValve.activeTime
#define FLUE_VALVE_OPEN_VALUE (config.configData.vacationMode.enabled ? config.configData.flueValve.closedValue : config.configData.flueValve.openValue)
#define FLUE_VALVE_CLOSED_VALUE config.configData.flueValve.closedValue
#define FLUE_VALVE_DEFAULT_VALUE (config.configData.vacationMode.enabled ? config.configData.flueValve.closedValue : config.configData.flueValve.defaultValue)

#define BOILER_VALVE_PIN config.configData.boilerValve.pin
#define BOILER_VALVE_SYNC_INTERVAL config.configData.boilerValve.syncInterval
#define BOILER_VALVE_ANTI_STALL_INTERVAL config.configData.boilerValve.antiStallInterval
#define BOILER_VALVE_ACTIVE_TIME config.configData.boilerValve.activeTime
#define BOILER_VALVE_OPEN_VALUE (config.configData.vacationMode.enabled ? config.configData.boilerValve.closedValue : config.configData.boilerValve.openValue)
#define BOILER_VALVE_CLOSED_VALUE config.configData.boilerValve.closedValue
#define BOILER_VALVE_DEFAULT_VALUE (config.configData.vacationMode.enabled ? config.configData.boilerValve.closedValue : config.configData.boilerValve.defaultValue)

//Heating state configuration 
#define MAX_OUTSIDE_TEMPERATURE config.configData.heatingState.maxOutsideTemperature
#define MIN_OUTSIDE_TEMPERATURE config.configData.heatingState.minOutsideTemperature

#define SECOND_HEATER_ENGAGE_TEMPERATURE config.configData.heatingState.secondHeaterEngageTemperature

#define HEATER_MAX_TEMPERATURE (config.configData.vacationMode.enabled ? config.configData.vacationMode.electricHeatingMaxTemperature : config.configData.heatingState.electricHeatingMaxTemperature)
#define HEATER_MIN_TEMPERATURE (config.configData.vacationMode.enabled ? config.configData.vacationMode.electricHeatingMinTemperature : config.configData.heatingState.electricHeatingMinTemperature)
#define HEATER_DELTA_TEMPERATURE (config.configData.vacationMode.enabled ? config.configData.vacationMode.electricHeatingDeltaTemperature : config.configData.heatingState.electricHeatingDeltaTemperature)

#define FLUE_COOLER_ON config.configData.heatingState.flueCoolerOnTemperature
#define MAX_ALLOWED_BOILER_TEMPERATURE config.configData.heatingState.maxAllowedBoilerTemperature

//Idle state configuration
#define IDLE_TIME_BETWEEN_FLOW_CHECKS config.configData.idleState.timeBetweenFlowChecks
#define IDLE_FLOW_CHECK_DURATION config.configData.idleState.idleFlowCheckDuration
#define IDLE_PUMP_HEALTHY_FLOW_THRESHOLD config.configData.idleState.pumpHealthyFlowThreshold

//Heating turn on / turn off transitions
#define TURN_ON_HEATING_TIME_BETWEEN_FLOW_CHECKS config.configData.heatingTurnOn.timeBetweenFlowChecks
#define TURN_ON_HEATING_FLOW_CHECK_DURATION config.configData.heatingTurnOn.flowCheckDuration
#define TURN_ON_HEATING_FLOW_THRESHOLD config.configData.heatingTurnOn.flowThreshold
#define TURN_ON_HEATING_TRY_TEMPERATURE_THRESHOLD (config.configData.vacationMode.enabled ? config.configData.vacationMode.heatingTurnOnMaxOutsideTemperature : config.configData.heatingTurnOn.maxOutsideTemperature)
#define TURN_ON_HEATING_TRY_INSITE_TEMPERATURE_THRESHOLD (config.configData.vacationMode.enabled ? config.configData.vacationMode.heatingTurnOnMaxInsideTemperature : config.configData.heatingTurnOn.maxInsideTemperature)
#define TURN_ON_HEATING_FORCE_TEMPERATURE_THRESHOLD config.configData.heatingTurnOn.outsideTemperatureToForceHeatingOn

#define TURN_OFF_HEATING_MAX_CONSUMED_POWER config.configData.heatingTurnOff.heatingPowerThreshold
#define TURN_OFF_HEATING_TRY_TEMPERATURE_THRESHOLD (config.configData.vacationMode.enabled ? config.configData.vacationMode.heatingTurnOffMinOutsideTemperature : config.configData.heatingTurnOff.minOutsideTemperature)
#define TURN_OFF_HEATING_FORCE_TEMPERATURE_THRESHOLD (config.configData.vacationMode.enabled ? config.configData.vacationMode.heatingTurnOffOutsideTemperatureToForceHeatingOff : config.configData.heatingTurnOff.outsideTemperatureToForceHeatingOff)

//Errors configuration
#define ERROR_BOILER_OVER_HEATING_THRESHOLD config.configData.errors.boilerOverheatingThreshold

#define PUMP_FAILURE_FLOW_THRESHOLD config.configData.errors.pumpFailureFlowThreshold
#define PUMP_FAILURE_MIN_TIME config.configData.errors.pumpFailureMinTime
#define PUMP_FAILURE_EXPIRATION config.configData.errors.pumpFailureExpiration



//////////////////////////////////////////////////////////////////////////////
// Hardware settings (pins to devices allocation)
// Prototype board to physical pin mapping. Named as "_XX", as conflicts with nodemcudefinitions
#define _D2       2  //LED
#define _D4       4
#define _D5       5 
#define _D9       9 
#define _D10      10 //SDA
#define _D12      12
#define _D13      13
#define _D14      14
#define _D16      16

//Device to board pin mapping
#define PIN_TERMO       _D5    // Pin for termo sensors only (including on board sensor)
#define PIN_DIGITAL_IO1 _D4    // General purpose digital IO pin, exposed outside
//#define PIN_DIGITAL_IO2 _D9    // General purpose digital IO pin, exposed outside - TODO - detect this from schematic
#define PIN_ANALOG_IN    A0    // Analog read

#define PIN_LED         _D2    // Onboard led
#define LED_OFF 1
#define LED_ON 0

//Pins for cummunication with serial to parallel out port adapter
#define PIN_SET     _D12           
#define PIN_CLK     _D13
#define PIN_DATA    _D14

//Change mapping when changing relay board. Current are turned on by zero on the input.
#define RELAY_OFF 0
#define RELAY_ON 1

#define ONE_WIRE_BUS PIN_TERMO
#define TEMPERATURE_PRECISION 10
#define ERROR_TEMP_VALUE 85

#endif //__SETTING_H__INCLUDED__