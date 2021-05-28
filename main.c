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
#define TIM 250
#define TPASO 100							//Tiempo entre pasos de arranque
#define PASOS_ARRANQUE 400
#define ERROR -1


int main(void)
{
	uint8_t cont = 0;
	uint16_t seq = INVERTER_COMM_SEQ1;
	uint32_t timer = 0;
	uint32_t tPaso = 0;
	uint32_t conteo = 0;
	
	GPIO_Config();
	//MCTM_Configuration();
	board_hardware_configuration();
	
	
	timer = board_scheduler_load_timer(TIM);
	tPaso = board_scheduler_load_timer(TPASO);
	
	//motor_3phase_start_motor();
	//motor_3phase_set_motor_direction(MOTOR_DIRECTION_FOWARD);
	
	
	
	while(1)
	{
		if(board_scheduler_is_time_expired(timer))
		{			
			timer = board_scheduler_load_timer(TIM);
			__hardware_gpio_output_toggle(GPIOC,GPIO3);
			//motor_3phase_starting_state_machine();
		}
		
		
		if(board_scheduler_is_time_expired(tPaso))
		{
			tPaso = board_scheduler_load_timer(TPASO-cont);
			inverter_3phase_comm_set_seq(seq,INVERTER_STATE_OVERWRITE);
			seq = SiguienteSeq(seq);
			//cont++;
			//cont %= TPASO;
		}
		
	}
}