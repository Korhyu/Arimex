#include "AL00_micro_ht32f52241.h"

#define HTCFG_MCTM_RELOAD 20000
#define HTCFG_MCTM_DEAD_TIME 0
#define DUTY 3/7


#define INVERTER_COMM_FREWHEEL		(1<<0)/*Todos los transistores abiertos*/
#define INVERTER_COMM_SEQ1				(1<<1)
#define INVERTER_COMM_SEQ2				(1<<2)
#define INVERTER_COMM_SEQ3				(1<<3)
#define INVERTER_COMM_SEQ4				(1<<4)
#define INVERTER_COMM_SEQ5				(1<<5)
#define INVERTER_COMM_SEQ6				(1<<6)
#define INVERTER_COMM_BREAK_LOW		(1<<7)/*Todos los transistores de arriba abiertos y todos los de abajo conduciendo*/
#define	INVERTER_COMM_BREAK_HIGH	(1<<8)/*Todos los transistores de arriba conduciendo y todos los de abajo abiertos*/
#define INVERTER_COMM_COUNT				(1<<9)

//Estados de las maquina de estados del arranque del motor
#define STARTING_STATE_INIT			 										0
#define STARTING_STATE_BREAK_AND_CHARGE_BOOTSTRAP		1
#define STARTING_STATE_ALIGMENT											2
#define STARTING_STATE_FIRST_STEPS_FROM_STAND				3
#define STARTING_STATE_FREEWHEEL_PERIOD_MEASURE			4
#define STARTING_STATE_FREEWHEEL_SYNC								5
#define STARTING_STATE_STEPS_CORRECT_TIMMING				6
#define STARTING_STATE_COUNT												8


void MCTM_Configuration(void);
void GPIO_Config (void);
void micro_config (void);
void SYSTICK_Configuration(void);

uint16_t SiguienteSeq (uint16_t);