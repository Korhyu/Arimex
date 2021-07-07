/* Includes ------------------------------------------------------------------------------------------------*/
#include "AL01_board_iq_aed_rev20191011.h"
#include "AL01_flash_management.h"
#include "AL02_inverter_3phase.h"
#include "AL02_switches_leds_mux_driver.h"
#include "AL03_motor_3phase_driver.h"
#include "Jose.h"

/* Global functions ----------------------------------------------------------------------------------------*/
/* Settings ------------------------------------------------------------------------------------------------*/
/* Private types -------------------------------------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------------------------------------*/
/* Global variables ----------------------------------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------------------------------------*/
/* Global functions ----------------------------------------------------------------------------------------*/


/*********************************************************************************************************//**
  * @brief  Main program.
  * @retval None
  ***********************************************************************************************************/
#define TIM 					250
#define TIM_BOOTSTRAP 200
#define TPASO 				15					//Tiempo entre pasos
#define TESTADO				100					//Tiempo en Freewheel en ms
#define REV_START 		10					//Vueltas de arranque
#define ERROR 				-1

#define START_SEQ				INVERTER_COMM_SEQ1
#define BOOTSTRAP_SEQ		INVERTER_COMM_BREAK_LOW

#define STOP				0						//Motor detenido
#define BOOTSTRAP		1						//Carga de bootstrap
#define STARTING 		2						//Arrancando el motor
#define RUNNING			3						//Medidos los periodos y paso a funcionamiento normal
#define FREEWHEEL 	4						//Motor en freewheel


int main(void)
{
	uint8_t revCount = 0;
	char seq = START_SEQ;
	uint32_t tEstado = 0;
	uint32_t tPaso = 0;
	uint32_t tBootstrap = 0;
	int estado = STOP;
	
	GPIO_Config();
	board_hardware_configuration();
	
	
	//Arranque Jose
	//En vez de esto deberia usar:
	//motor_3phase_init();
	//motor_3phase_starting_state_machine();
	
	while(1)
	{
		switch(estado)
		{
			case STOP:
				inverter_3phase_comm_set_seq(INVERTER_COMM_BREAK_LOW, INVERTER_STATE_OVERWRITE);
				tBootstrap = board_scheduler_load_timer(TIM_BOOTSTRAP);
				estado = BOOTSTRAP;
				break;
				
			
			case BOOTSTRAP:
				if(board_scheduler_is_time_expired(tBootstrap))
				{
					inverter_3phase_comm_set_seq(seq,INVERTER_STATE_OVERWRITE);
					tPaso = board_scheduler_load_timer(TPASO);
					seq = SiguienteSeq(seq);
					estado = STARTING;
				}
				break;
		
			
			case STARTING:
				//Secuencia de arranque
				if(board_scheduler_is_time_expired(tPaso))
				{
					tPaso = board_scheduler_load_timer(TPASO-revCount);
					inverter_3phase_comm_set_seq(seq,INVERTER_STATE_OVERWRITE);
					seq = SiguienteSeq(seq);
					if (seq == START_SEQ && revCount <= REV_START)
					{
						//Di una vuelta y paso a cambiar la frecuencia de giro
						revCount++;
					}
					//Si llegue a las vueltas de arranque paso a freewheel
					if (revCount >= REV_START)
					{
						__hardware_gpio_output_set(GPIOA, 3);					//GPIO aux para monitoreo en OSC
						estado = RUNNING;															//FREEWHEEL
						inverter_3phase_comm_set_seq(INVERTER_COMM_FREWHEEL,INVERTER_STATE_OVERWRITE);
						tEstado = board_scheduler_load_timer(TESTADO);
					}
				}
				break;
				
			case RUNNING:
				if(board_scheduler_is_time_expired(tPaso))
				{
					tPaso = board_scheduler_load_timer(TPASO-revCount);
					inverter_3phase_comm_set_seq(seq,INVERTER_STATE_OVERWRITE);
					seq = SiguienteSeq(seq);
					/*
					if (seq == START_SEQ)
					{
						//Di una vuelta
						revCount++;
					}
					*/
				}
				break;
				
				
			case FREEWHEEL:
				//Pongo el estado Freewheel para medir BEMF y demas con el OSC
				//Luego de TESTADO reinicio el arranque
				if(board_scheduler_is_time_expired(tEstado))
				{
					estado = STARTING;
					inverter_3phase_comm_set_seq(seq,INVERTER_STATE_OVERWRITE);
					revCount = 0;
					__hardware_gpio_output_reset(GPIOA, 3);					//GPIO aux para monitoreo en OSC
					tPaso = board_scheduler_load_timer(TPASO);
				}
			
				break;
		}
	}
}