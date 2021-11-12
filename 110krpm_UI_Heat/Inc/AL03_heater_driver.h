#include "AL02_inverter_3phase.h"


#define HEATER_ON_STATE					1
#define HEATER_OFF_STATE				2
#define HEATER_WAITING_STATE			3

#define HEATER_CONFIG_STATE				10



#define heater_outputs_set()			{__hardware_gpio_output_set(BOARD_HEATER_GATE1_PORT,BOARD_HEATER_GATE1_PIN);\
										 __hardware_gpio_output_set(BOARD_HEATER_GATE2_PORT,BOARD_HEATER_GATE2_PIN);}
#define heater_outputs_reset()			{__hardware_gpio_output_reset(BOARD_HEATER_GATE1_PORT,BOARD_HEATER_GATE1_PIN);\
										 __hardware_gpio_output_reset(BOARD_HEATER_GATE2_PORT,BOARD_HEATER_GATE2_PIN);}

#define heater_output_1_set()			__hardware_gpio_output_set(BOARD_HEATER_GATE1_PORT,BOARD_HEATER_GATE1_PIN);
#define heater_output_1_reset()			__hardware_gpio_output_reset(BOARD_HEATER_GATE1_PORT,BOARD_HEATER_GATE1_PIN);
#define heater_output_2_set()			__hardware_gpio_output_set(BOARD_HEATER_GATE2_PORT,BOARD_HEATER_GATE2_PIN);
#define heater_output_2_reset()			__hardware_gpio_output_reset(BOARD_HEATER_GATE2_PORT,BOARD_HEATER_GATE2_PIN);


#define heater_outputs_state_init()		{__hardware_gpio_output_reset(BOARD_HEATER_GATE1_PORT,BOARD_HEATER_GATE1_PIN);\
										 __hardware_gpio_output_reset(BOARD_HEATER_GATE2_PORT,BOARD_HEATER_GATE2_PIN);}


#define heater_zcd_pin_get_state()		__hardware_gpio_input_read_state(BOARD_HEATER_ZCD_DET_PORT,BOARD_HEATER_ZCD_DET_PIN)


void adc_callback_handler (void);