/* Includes ------------------------------------------------------------------------------------------------*/
#include "AL01_board_iq_aed_rev20191011.h"
#include "AL01_flash_management.h"
#include "AL02_inverter_3phase.h"
#include "AL02_switches_leds_mux_driver.h"
#include "AL03_motor_3phase_driver.h"
#include "AL03_ui_driver.h"

/* Global functions ----------------------------------------------------------------------------------------*/
/* Settings ------------------------------------------------------------------------------------------------*/
/* Private types -------------------------------------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------------------------------------*/
/* Global variables ----------------------------------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------------------------------------*/
/* Global functions ----------------------------------------------------------------------------------------*/

uint32_t ui_timer = 0;
int aux = 1;

int main(void)
{	
	//GPIO_Config();
	board_hardware_configuration();

	//Arranque
	//motor_3phase_init();
	//motor_3phase_start_motor();
	
	TM_IntConfig(HT_MCTM0,TM_INT_BRKEV, ENABLE);

	//printf("Hola, soy Icu, tu asistente virtual\n");

	//Leds de estado
	/*
	__hardware_gpio_output_set(GPIOA, 6);		//LED X
	__hardware_gpio_output_set(GPIOB, 12);		//LED X
	__hardware_gpio_output_set(GPIOC, 3);		//LED X
	__hardware_gpio_output_set(GPIOC, 6);		//LED X
	__hardware_gpio_output_set(GPIOC, 7);		//LED X
	__hardware_gpio_output_set(GPIOA, 8);		//LED X
	
	__hardware_gpio_output_reset(GPIOB, 10);	//Pin Bajo LEDs
	__hardware_gpio_output_reset(GPIOB, 14);	//Pin Bajo LEDs
	__hardware_gpio_output_reset(GPIOB, 0);		//Pin Bajo LEDs
	__hardware_gpio_output_reset(GPIOA, 15);	//Pin Bajo LEDs
	*/
	ui_timer = board_scheduler_load_timer(500);
	
	while(1)
	{
		//motor_3phase_task();

		ui_task();

		if (board_scheduler_is_time_expired(ui_timer))
		{
			if( aux < 0b100000 )
			{
				aux = aux << 1;
			}
			else
			{
				aux = 1;
			}

			ui_mux_set_leds_state(aux);
			ui_timer = board_scheduler_load_timer(500);
		}
		
	}
}