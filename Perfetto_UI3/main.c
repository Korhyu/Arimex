/* Includes ------------------------------------------------------------------------------------------------*/
#include "AL01_board_iq_aed_rev20191011.h"
#include "AL01_flash_management.h"
#include "AL02_inverter_3phase.h"
#include "AL02_switches_leds_mux_driver.h"
#include "AL03_motor_3phase_driver.h"
#include "AL03_ui_interaction.h"


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
static int32_t 	timer_ui = 0;		//Auxiliar para conteo de tiempo en UI
static int8_t	sw_command = 0;
uint8_t var=0b00111111;
uint8_t aux=0;
int32_t ui_timer=0;

int main(void)
{	
	//GPIO_Config();
	board_hardware_configuration();

	// UI Config
	ui_mux_init();
	
	//Arranque Fran
	motor_3phase_init();
	motor_3phase_start_motor();

	printf("Hola, soy Icu, tu asistente virtual\n");
	
	while(1)
	{
		motor_3phase_task();

		ui_task();
		
		if(ui_timer==0 || board_scheduler_is_time_expired(ui_timer))
		{
			ui_timer = board_scheduler_load_timer(TIME_REFRESH_mS);
			ui_update();
		}
	}
}