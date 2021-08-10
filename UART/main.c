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
#define TIM 				2000



int main(void)
{	
	board_hardware_configuration();


	uint32_t timer=0;
	

	timer = board_scheduler_load_timer(TIM);
	
	
	printf("Borro bien la flash\n");
	printf("Hubo un error al borrar la flash\n");
	printf("flash vacia\n");
	printf("flash no vacia\n");
	
	printf("flash vacia\n");
	printf("flash no vacia\n");
	
	

	while(1)
	{
		if(board_scheduler_is_time_expired(timer))
		{			
			timer = board_scheduler_load_timer(TIM);
		
			printf("Estoy vivo y el systick esta en:%d\n\r", timer);
		}
	}
}