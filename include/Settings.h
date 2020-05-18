#ifndef __SETTING_H__INCLUDED__
#define __SETTING_H__INCLUDED__

////////////////////////////////////////////////////////////////////////////
// Tunable settings (to be tuned and changes for each board)

// Timings
#define BUS_SYNC_DELAY     1000       //Delay between resyncing state of the parallel bus
#define TERMO_SYNC_DELAY   10000      //Delay between temperaature sensor readings

//Valves configuration (controlled by servos)
#define FLUE_VALVE_PIN                    5        //B6
#define FLUE_VALVE_SYNC_INTERVAL          20000    //Each 20 seconds (needs to be interactive to control overheating)
#define FLUE_VALVE_ANTI_STALL_INTERVAL    86400000 //Each 24 hours = 24 * 60 * 60 * 1000 ms
#define FLUE_VALVE_ACTIVE_TIME            1000     //Time to power the servo (should be enough to rotate)        
#define FLUE_VALVE_CLOSED_VALUE           80       //Value when flue valse is closed
#define FLUE_VALVE_DEFAULT_VALUE          40       //Value when flue is partially open  (default for heating)

#define BOILER_VALVE_PIN                   6         //B7
#define BOILER_VALVE_SYNC_INTERVAL         1800000   //Each 30 minutes = 30 * 60 * 60 * 1000 ms
#define BOILER_VALVE_ANTI_STALL_INTERVAL   604800000 //Each 7 days = 7 * 24 * 60 * 60 * 1000 ms
#define BOILER_VALVE_ACTIVE_TIME           1000      //Time to power the servo (should be enough to rotate)
#define BOILER_VALVE_CLOSED_VALUE          20        //Value when boiler valve is closed (min temperature)
#define BOILER_VALVE_DEFAULT_VALUE         40        //Default value, when heating is needed (before fire has been detected)

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
#define PIN_DIGITAL_IO  _D4    // General purpose digital IO pin, exposed outside
#define PIN_ANALOG_IN    A0    // Analog read

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