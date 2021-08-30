/* Includes ------------------------------------------------------------------------------------------------*/
#include "AL01_board_iq_aed_rev20191011.h"
#include "AL01_flash_management.h"
#include "AL02_inverter_3phase.h"
#include "AL02_switches_leds_mux_driver.h"
#include "AL03_motor_3phase_driver.h"
//#include "Jose.h"

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
#define TIM 			250
#define TIME_UI_mS		100			//Tiempo entre verificaciones de cambios de la UI


static int32_t 	timer_ui = 0;		//Auxiliar para conteo de tiempo en UI
static int8_t	sw_command = 0;

int main(void)
{	
	//GPIO_Config();
	board_hardware_configuration();

	// UI Config
	ui_mux_init();
	
	//Arranque Fran
	motor_3phase_init();
	motor_3phase_start_motor();
	
	TM_IntConfig(HT_MCTM0,TM_INT_BRKEV,ENABLE);
	//TM_IntConfig(HT_MCTM0,TM_INT_CH2CC,DISABLE);
	//__hardware_gpio_output_set(GPIOA, 3);					//GPIO aux para monitoreo en OSC

	printf("Hola, soy Icu, tu asistente virtual\n");
	
	while(1)
	{
		motor_3phase_task();


		//Scheduler para control de estado de los switches
		if(timer_ui==0 || board_scheduler_is_time_expired(timer_ui))
		{
			sw_command = switches_status();

			if(sw_command != NO_CHANGE_SW)
			{
				switch (sw_command)
				{
					case COLDSHOT_ON_SW:

										break;

					case COLDSHOT_OFF_SW:

										break;
					
					case LOCK_ON_SW:

										break;

					case LOCK_OFF_SW:

										break;

					case MOTOR_UP_SW:

										break;
					
					case MOTOR_DOWN_SW:

										break;
					
					case HEAT_UP_SW:

										break;

					case HEAT_DOWN_SW:

										break;
					
					default:			break;
				}

				//Luego de hacer la accion correspondiente hay que reiniciar la variable para que no haga 2 veces la misma accion accidentalmente
				//Pienso que posponer la accion por el doble de tiempo es suficiente para descartar "detecciones accidentales"
				timer_ui = board_scheduler_load_timer(TIME_UI_mS * 2);
			}
			else
			{
				//Si no hay cambio reviso de nuevo en un tiempo para ver si lo hubo
				timer_ui = board_scheduler_load_timer(TIME_UI_mS);
			}
		}
		
		
	
		//__hardware_gpio_output_toggle(GPIOB, 11);
	}
}