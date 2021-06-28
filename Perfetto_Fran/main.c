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
#define TPASO 			170					//Tiempo entre pasos
#define REV_START 	70					//Vueltas de arranque
#define ERROR 			-1



int main(void)
{	
	//GPIO_Config();
	board_hardware_configuration();
	
	//Arranque Fran
	motor_3phase_init();
	motor_3phase_start_motor();
	
	
	TM_IntConfig(HT_MCTM0,TM_INT_BRKEV,ENABLE);
	//TM_IntConfig(HT_MCTM0,TM_INT_CH2CC,DISABLE);
	__hardware_gpio_output_set(GPIOA, 3);					//GPIO aux para monitoreo en OSC
	
	while(1)
	{
		motor_3phase_task();
		
		
		//__hardware_gpio_output_toggle(GPIOB, 11);
	}
}