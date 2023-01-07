#ifndef __SETTING_H__INCLUDED__
#define __SETTING_H__INCLUDED__

////////////////////////////////////////////////////////////////////////////
// Tunable settings (to be tuned and changes for each board)

// Timings
#define BUS_SYNC_DELAY     1000       //Delay between resyncing state of the parallel bus
#define TERMO_SYNC_DELAY   2000      //Delay between temperaature sensor readings

//Valves configuration (controlled by servos)
#define FLUE_VALVE_PIN                    6        //A7
#define FLUE_VALVE_SYNC_INTERVAL          20000    //Each 20 seconds (needs to be interactive to control overheating)
#define FLUE_VALVE_ANTI_STALL_INTERVAL    86400000 //Each 24 hours = 24 * 60 * 60 * 1000 ms
#define FLUE_VALVE_ACTIVE_TIME            1000     //Time to power the servo (should be enough to rotate)        
#define FLUE_VALVE_OPEN_VALUE             0        //Value when flue valse is fully open
#define FLUE_VALVE_CLOSED_VALUE           100      //Value when flue valse is closed
#define FLUE_VALVE_DEFAULT_VALUE          0        //Value when flue is partially open  (default for heating)

#define BOILER_VALVE_PIN                   7         //A8
#define BOILER_VALVE_SYNC_INTERVAL         600000    //Each 10 minutes = 10 * 60 * 60 * 1000 ms
#define BOILER_VALVE_ANTI_STALL_INTERVAL   604800000 //Each 7 days = 7 * 24 * 60 * 60 * 1000 ms
#define BOILER_VALVE_ACTIVE_TIME           1000      //Time to power the servo (should be enough to rotate)
#define BOILER_VALVE_OPEN_VALUE            0         //Value when boiler valve is open (max temperature)
#define BOILER_VALVE_CLOSED_VALUE          100       //Value when boiler valve is closed (min temperature)
#define BOILER_VALVE_DEFAULT_VALUE         50        //Default value, when heating is needed (before fire has been detected)

//Heating configuration 
#define MAX_OUTSITE_TEMPERATURE 20.0
#define MIN_OUTSIDE_TEMPERATURE -10.0

#define SECOND_HEATER_ENGAGE_TEMPERATURE 5.0

#define HEATER_MAX_TEMPERATURE  55.0  //Max working temperature for electric heater
#define HEATER_MIN_TEMPERATURE  30.0  //Min working temperature for electric heater
#define HEATER_DELTA_TEMPERATURE 2.5  //Delta around working heater temperature which is allowed

#define FLUE_COOLER_ON  110  //Flue temperature to start cooler to prevent teprerature sensors overheating
#define MAX_ALLOWED_BOILER_TEMPERATURE 75 //Max builder temperature which does not require intervention 

//Idle state configuration
#define IDLE_TIME_BETWEEN_FLOW_CHECKS 604800000 //Each 7 days = 14 * 24 * 60 * 60 * 1000 ms
#define IDLE_FLOW_CHECK_DURATION      60000     //1 minute = 1 * 60 * 1000 ms
#define IDLE_PUMP_HEALTHY_FLOW_THRESHOLD 8.0

//Heating turn on / turn off transitions
#define TURN_ON_HEATING_TIME_BETWEEN_FLOW_CHECKS    7200000   //Check pump each 2 hours  
#define TURN_ON_HEATING_FLOW_CHECK_DURATION         30000     //Measure flow for long enough
#define TURN_ON_HEATING_FLOW_THRESHOLD              12.0      //Value above this triggers heating enabling (some of heating devices are open for heating) 
#define TURN_ON_HEATING_TRY_TEMPERATURE_THRESHOLD   18.0      //Min temperature to start checking the need for heating
#define TURN_ON_HEATING_TRY_INSITE_TEMPERATURE_THRESHOLD   24.0      //Inside temperature to try heating
#define TURN_ON_HEATING_FORCE_TEMPERATURE_THRESHOLD 1.0       //Min temperature to start heating unconditionally

#define TURN_OFF_HEATING_MAX_CONSUMED_POWER          0.5       //Min power consumed by the house to turn off heating
#define TURN_OFF_HEATING_TRY_TEMPERATURE_THRESHOLD   8.0       //Min temperature to turn off heating
#define TURN_OFF_HEATING_FORCE_TEMPERATURE_THRESHOLD 20.0      //Min temperature to stop heating unconditionally

//Errors configuration
#define ERROR_BOILER_OVER_HEATING_THRESHOLD 85.0  //Min temperature above which boiler is considered overheated

#define PUMP_FAILURE_FLOW_THRESHOLD 1.0     //Min flow which is valid for pump
#define PUMP_FAILURE_MIN_TIME       10000   //Min time pump should be active to check for error
#define PUMP_FAILURE_EXPIRATION     5000    //Min time to expire error (so that short pump turn on / off do not disable this error)   



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