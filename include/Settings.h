#ifndef __SETTING_H__INCLUDED__
#define __SETTING_H__INCLUDED__

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
#define PIN_DIGITAL_IO2 _D9    // General purpose digital IO pin, exposed outside
//#define PIN_ANALOG_IN    A0    // Analog read - not used in the current board

//Pins for cummunication with serial to parallel out port adapter
#define PIN_SET     _D12           
#define PIN_CLK     _D13
#define PIN_DATA    _D14
#define ADDR_LEN    8
#define STATE_LEN   8

#define BUS_BIT_1   7
#define BUS_BIT_2   6
#define BUS_BIT_3   5
#define BUS_BIT_4   4
#define BUS_BIT_5   3
#define BUS_BIT_6   2
#define BUS_BIT_7   1
#define BUS_BIT_8   0

#define ALL_DIGITAL_PINS {PIN_TERMO, PIN_DIGITAL_IO1, PIN_DIGITAL_IO2, PIN_SET, PIN_CLK, PIN_DATA}

//Change mapping when changing relay board. Current are turned on by zero on the input.
#define RELAY_OFF 0
#define RELAY_ON 1

#define ONE_WIRE_BUS PIN_TERMO
#define TEMPERATURE_PRECISION 10
#define ERROR_TEMP_VALUE 85

////////////////////////////////////////////////////////////////////////////
// Tunable settings (to be tuned and changes for each board)

// Temperature settings
#define HEATING_PUMP_ON 35   //Temperature when heating pump starts
#define HEATING_PUMP_OFF 30  //Temperature when heating pump stops (to prevent heating loss)

#define ANTIFREEZE_PUMP_ON  5  //Temperature when pump start to prevent boiler freeze
#define ANTIFREEZE_PUMP_OFF 10 //Temperature when pump stops (freezing has been prevented)

//TODO - this heater does not work. Reconfigure when fixed
#define HEATER_1_ON  0 //Temperature to turn on the first heater
#define HEATER_1_OFF 5 //Temperature to turn off the first heater

#define HEATER_2_ON  15  //Temperature to turn on the second heater
#define HEATER_2_OFF 40  //Temperature to turn off the first heater

#define HEATER_3_ON  15  //Temperature to turn on the third heater 
#define HEATER_3_OFF 43  //Temperature to turn off the first heater

#define BOILER_COOLER_ON 85    //Boiler temperature to start cooler to prevent overheating
#define BOILER_COOLER_OFF 75   //Boiler temperature to stop cooler 

#define FLUE_COOLER_ON  110  //Flue temperature to start cooler to prevent teprerature sensors overheating
#define FLUE_COOLER_OFF 75   //Flue temperature to stop cooler 

// Timings
#define BUS_SYNC_DELAY     1000       //Delay between resyncing state of the parallel bus
#define TERMO_SYNC_DELAY   10000      //Delay between temperaature sensor readings
#define PUMP_CHECK_DELAY   10000      //Delay between checks if pump relay is in a correct state

//Valves configuration (controlled by servos)
#define FLUE_VALVE_PIN                    BUS_BIT_6  //B6
#define FLUE_VALVE_SYNC_INTERVAL          20000      //Each 20 seconds (needs to be interactive to control overheating)
#define FLUE_VALVE_ANTI_STALL_INTERVAL    86400000   //Each 24 hours = 24 * 60 * 60 * 1000 ms
#define FLUE_VALVE_ACTIVE_TIME            1000       //Time to power the servo (should be enough to rotate)        

#define BOILER_VALVE_PIN                   BUS_BIT_7 //B7
#define BOILER_VALVE_SYNC_INTERVAL         1800000   //Each 30 minutes = 30 * 60 * 60 * 1000 ms
#define BOILER_VALVE_ANTI_STALL_INTERVAL   604800000 //Each 7 days = 7 * 24 * 60 * 60 * 1000 ms
#define BOILER_VALVE_ACTIVE_TIME           1000      //Time to power the servo (should be enough to rotate)        

#endif //__SETTING_H__INCLUDED__