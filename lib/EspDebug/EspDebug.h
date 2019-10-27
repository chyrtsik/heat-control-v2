#ifndef __ESP_DEBUG_INCLUDED__
#define __ESP_DEBUG_INCLUDED__
// Debugging defines for ESP

//Uncomment if not defined outside
//#define DEBUG_ESP_PORT Serial

#ifdef DEBUG_ESP_PORT
#define DEBUG_INIT() DEBUG_ESP_PORT.begin(115200)
#define DEBUG_PRINT_TIME() DEBUG_ESP_PORT.print(millis()); DEBUG_ESP_PORT.print(": ") 
#define DEBUG_PRINTF(...) DEBUG_PRINT_TIME(); DEBUG_ESP_PORT.printf(__VA_ARGS__)
#define DEBUG_PRINT(message) DEBUG_PRINT_TIME(); DEBUG_ESP_PORT.print(message)
#define DEBUG_PRINT_LN(message) DEBUG_PRINT_TIME(); DEBUG_ESP_PORT.println(message)
#else
#define DEBUG_INIT()
#define DEBUG_PRINTF(...)
#define DEBUG_PRINT(message)
#define DEBUG_PRINT_LN(message)
#endif

#endif //__ESP_DEBUG_INCLUDED__