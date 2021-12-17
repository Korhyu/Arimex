#include "AL02_switches_leds_mux_driver.h"


#define UI_WAITING_TIMER_STATE		1
#define UI_LEDS_STATE				2
#define UI_SWITCHES_STATE			3
#define UI_UPDATE_VARIABLES			4

#define UI_CONFIG_STATE				10

#define UI_ACTION_REQUIRED			1
#define UI_NO_ACTION_REQUIRED		2


void ui_task (void);					//Maquina de estados de la UI
void ui_update (void);					//Actualiza la UI
void change_state_to_leds (void);		//Configura los pines para escribir los LEDs
void change_state_to_switches (void);	//Configura los pines para leer los SW
void ui_mux_callback(void);				//Calback del timer - SIN USO
void ui_led_refresh	(void);				//Pide el estado del motor y calefa y lo escribe en los LEDs
int32_t get_switches_state (void);		//Lee el estado de los SW