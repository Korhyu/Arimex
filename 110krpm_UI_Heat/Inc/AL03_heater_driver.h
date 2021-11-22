#include "AL02_inverter_3phase.h"


#define HEATER_SWITCHING_STATE			1
#define HEATER_OFF_STATE				2
#define HEATER_TIME_MACHINE_STATE		3
#define HEATER_LOAD_STATE				4

#define HEATER_CONFIG_STATE				10

#define HEATER_DEFAULT_PERIOD			20		//Cantidad de ticks que dura el periodo de los calefa

#define BRANCH_OFF						0
#define BRANCH_ON						1


//Estos defines corresponden al pulso de exitacion. NO AL TIEMPO DE LOS CALEFAS, SOLO A LA EXITACION DE LOS TRIACS
#define HEATER_PULSE_TOFF				8		//Tiempo de TOFF del pulso de exitacion de los TRIACS
#define HEATER_PULSE_TON				19		//Tiempo de TON del pulso de exitacion de los TRIACS

#define heater_outputs_set()			{__hardware_gpio_output_set(BOARD_HEATER_GATE1_PORT,BOARD_HEATER_GATE1_PIN);\
										 __hardware_gpio_output_set(BOARD_HEATER_GATE2_PORT,BOARD_HEATER_GATE2_PIN);}
#define heater_outputs_reset()			{__hardware_gpio_output_reset(BOARD_HEATER_GATE1_PORT,BOARD_HEATER_GATE1_PIN);\
										 __hardware_gpio_output_reset(BOARD_HEATER_GATE2_PORT,BOARD_HEATER_GATE2_PIN);}

#define heater_b1_set()					__hardware_gpio_output_set(BOARD_HEATER_GATE1_PORT,BOARD_HEATER_GATE1_PIN);
#define heater_b1_reset()				__hardware_gpio_output_reset(BOARD_HEATER_GATE1_PORT,BOARD_HEATER_GATE1_PIN);
#define heater_b2_set()					__hardware_gpio_output_set(BOARD_HEATER_GATE2_PORT,BOARD_HEATER_GATE2_PIN);
#define heater_b2_reset()				__hardware_gpio_output_reset(BOARD_HEATER_GATE2_PORT,BOARD_HEATER_GATE2_PIN);


#define heater_outputs_state_init()		{__hardware_gpio_output_reset(BOARD_HEATER_GATE1_PORT,BOARD_HEATER_GATE1_PIN);\
										 __hardware_gpio_output_reset(BOARD_HEATER_GATE2_PORT,BOARD_HEATER_GATE2_PIN);}


#define heater_zcd_pin_get_state()		__hardware_gpio_input_read_state(BOARD_HEATER_ZCD_DET_PORT,BOARD_HEATER_ZCD_DET_PIN)


void heater_task (void);