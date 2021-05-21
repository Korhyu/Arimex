/* Includes ------------------------------------------------------------------------------------------------*/
#include "AL01_board_iq_aed_rev20191011.h"
#include "AL01_flash_management.h"
#include "AL02_inverter_3phase.h"
#include "AL02_switches_leds_mux_driver.h"

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
#define LED1_PORT GPIOC
#define LED1_PIN	GPIO_PIN_3
#define LED1_NUMB	GPIO3


int main(void)
{
	uint8_t var=1;
	uint32_t timer=0;
	
  board_hardware_configuration();
	//ui_mux_init();
	//inverter_3phase_init_config();

	
	board_hardware_gpio_config_output_pp_pins_load_config		(LED1_PORT,GPIO_PIN_3);
	__hardware_gpio_output_reset(LED1_PORT,GPIO3);
	
	timer= board_scheduler_load_timer(TIM);
	
	
	
	/*
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
	*/
	

	while(1)
	{
		if(board_scheduler_is_time_expired(timer))
		{			
			timer= board_scheduler_load_timer(TIM);
			__hardware_gpio_output_toggle(LED1_PORT,GPIO3);
			
			/*
			printf("switches:%d\n\r",ui_mux_get_switches_state());
			if(var>=0x40)
				var=1;
			else
				var<<=1;
			
			ui_mux_set_leds_state(var);
			*/
			

		}
	}
}