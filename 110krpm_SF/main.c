/* Includes ------------------------------------------------------------------------------------------------*/
#include "AL01_board_iq_aed_rev20191011.h"
#include "AL01_flash_management.h"
#include "AL02_inverter_3phase.h"
#include "AL02_switches_leds_mux_driver.h"
#include "AL03_motor_3phase_driver.h"

/* Global functions ----------------------------------------------------------------------------------------*/
/* Settings ------------------------------------------------------------------------------------------------*/
/* Private types -------------------------------------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------------------------------------*/
/* Global variables ----------------------------------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------------------------------------*/
/* Global functions ----------------------------------------------------------------------------------------*/



int main(void)
{	
	//GPIO_Config();
	board_hardware_configuration();

	// UI Config
	//ui_mux_init();
	
	//Arranque
	motor_3phase_init();
	motor_3phase_start_motor();
	
	TM_IntConfig(HT_MCTM0,TM_INT_BRKEV,ENABLE);

	printf("Hola, soy Icu, tu asistente virtual\n");
	
	while(1)
	{
		motor_3phase_task();
	}
}