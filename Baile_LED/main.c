/* Includes ------------------------------------------------------------------------------------------------*/
#include "AL01_board_iq_aed_rev20191011.h"


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
#define tim 50


#include "AL02_switches_leds_mux_driver.h"
#include "AL01_flash_management.h"

int main(void)
{
	uint8_t var=1;
	

	uint32_t timer=0;
	
  board_hardware_configuration();

	timer= board_scheduler_load_timer(tim);
	
	ui_mux_init();
	
	if(flash_user_store_erase() == FLASH_COMPLETE)
		printf("Borro bien la flash\n");
	else
		printf("Hubo un error al borrar la flash\n");
	
	if(flash_user_is_the_flash_empty())
		printf("flash vacia\n");
	else
		printf("flash no vacia\n");
	
	flash_user_write_1byte_at_offset(0,0);
	
	if(flash_user_is_the_flash_empty())
		printf("flash vacia\n");
	else
		printf("flash no vacia\n");
	
	

	while(1)
	{
		if(board_scheduler_is_time_expired(timer))
		{			
			timer= board_scheduler_load_timer(tim);
		
			printf("switches:%d\n\r",ui_mux_get_switches_state());
			
			if(var>=0x40)
				var=1;
			else
				var<<=1;
			
			ui_mux_set_leds_state(var);

		}
	}
}
