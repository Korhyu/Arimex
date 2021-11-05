#include "AL02_switches_leds_mux_driver.h"


#define UI_WAITING_TIMER_STATE		1
#define UI_LEDS_STATE				2
#define UI_SWITCHES_STATE			3
#define UI_UPDATE_VARIABLES			4

#define UI_CONFIG_STATE				10

#define UI_ACTION_REQUIRED			1
#define UI_NO_ACTION_REQUIRED		2



void ui_task (void);
int32_t get_switches_state (void);
void change_state_to_leds (void);
void change_state_to_switches (void);
void ui_mux_callback(void);