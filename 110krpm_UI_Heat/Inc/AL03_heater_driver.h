#include "AL02_inverter_3phase.h"


#define HEATER_SWITCHING_STATE			1
#define HEATER_WAITING_STATE			2
#define HEATER_CONTROL_STATE			3
#define HEATER_OFF_STATE				4

#define HEATER_CONFIG_STATE				10

#define HEATER_DEFAULT_PERIOD			50		//Cantidad de ticks que dura el periodo de los calefa

#define BRANCH_OFF						0
#define BRANCH_ON						1


#define HEATER_LEVEL_0					0
#define HEATER_LEVEL_1					1
#define HEATER_LEVEL_2					2
#define HEATER_LEVEL_3					3

//Estos defines corresponden al pulso de exitacion. NO AL TIEMPO DE LOS CALEFAS, SOLO A LA EXITACION DE LOS TRIACS
#define HEATER_PULSE_TON				14							//Tiempo de TON del pulso de exitacion de los TRIACS
#define HEATER_PULSE_TOFF				(20-HEATER_PULSE_TON)		//Tiempo de TOFF del pulso de exitacion de los TRIACS

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


void 	heater_task		(void);
void	select_seq		(uint32_t, uint32_t);
void	load_seq		(uint8_t);



static uint8_t b1_table[10][HEATER_DEFAULT_PERIOD] = {
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},	//Calor 0
	{1,0,1,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0},	//Velocidad 1 - Calor 1
	{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,0,0,0,1,0,1,0,0},	//Velocidad 1 - Calor 2
	{1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},	//Velocidad 1 - Calor 3
	{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0},	//Velocidad 2 - Calor 1
	{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,0},	//Velocidad 2 - Calor 2
	{1,0,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,0},	//Velocidad 2 - Calor 3
	{1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},	//Velocidad 3 - Calor 1
	{1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0},	//Velocidad 3 - Calor 2
	{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0}	//Velocidad 3 - Calor 3
	};	

static uint8_t b2_table[10][HEATER_DEFAULT_PERIOD] = {
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},	//Calor 0
	{0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0,1},	//Velocidad 1 - Calor 1
	{0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,1},	//Velocidad 1 - Calor 2
	{0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1},	//Velocidad 1 - Calor 3
	{0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1},	//Velocidad 2 - Calor 1
	{0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1},	//Velocidad 2 - Calor 2
	{0,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1},	//Velocidad 2 - Calor 3
	{0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1},	//Velocidad 3 - Calor 1
	{0,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1},	//Velocidad 3 - Calor 2
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}	//Velocidad 3 - Calor 3
	};


