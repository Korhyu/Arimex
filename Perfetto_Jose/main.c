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
#define TIM 				250
#define TPASO 			15					//Tiempo entre pasos
#define REV_START 	10					//Vueltas de arranque
#define ERROR 			-1



int main(void)
{
	uint8_t revCount = 0;
	char seq = INVERTER_COMM_SEQ1;
	uint32_t timer = 0;
	uint32_t tPaso = 0;
	uint32_t conteo = 0;
	
	GPIO_Config();
	board_hardware_configuration();
	
	timer = board_scheduler_load_timer(TIM);
	tPaso = board_scheduler_load_timer(TPASO);
	
	//Arranque Jose
	// En vez de esto deberia usar:
	//motor_3phase_init();
	//motor_3phase_starting_state_machine();
	while(1)
	{
		if(board_scheduler_is_time_expired(tPaso))
		{
			tPaso = board_scheduler_load_timer(TPASO-revCount);
			inverter_3phase_comm_set_seq(seq,INVERTER_STATE_OVERWRITE);
			seq = SiguienteSeq(seq);
			if (seq == INVERTER_COMM_SEQ1 && revCount <= REV_START)
			{
				//Di una vuelta y paso a cambiar la frecuencia de giro
				revCount++;
			}
		}
	}
}