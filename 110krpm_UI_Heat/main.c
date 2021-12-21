/* Includes ------------------------------------------------------------------------------------------------*/
#include "AL01_board_iq_aed_rev20191011.h"
#include "AL01_flash_management.h"
#include "AL02_inverter_3phase.h"
#include "AL02_switches_leds_mux_driver.h"
#include "AL03_motor_3phase_driver.h"
#include "AL03_heater_driver.h"
#include "AL03_vbus_control.h"
#include "AL03_ui_driver.h"
#include "AL03_ui_driver.h"
#include "AL04_supervisor.h"

/* Global functions ----------------------------------------------------------------------------------------*/
/* Settings ------------------------------------------------------------------------------------------------*/
/* Private types -------------------------------------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------------------------------------*/
/* Global variables ----------------------------------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------------------------------------*/
/* Global functions ----------------------------------------------------------------------------------------*/

int aux = 1;

int main(void)
{	
	//GPIO_Config();
	board_hardware_configuration();

	//Arranque
	motor_3phase_init();
	motor_3phase_start_motor();
	
	TM_IntConfig(HT_MCTM0,TM_INT_BRKEV, ENABLE);

	//printf("Hola, soy Icu, tu asistente virtual\n");

	while(1)
	{
		//supervisor_task();		//Encargado de la comunicacion entre todas las tareas
		
		motor_3phase_task();		//Tarea de control del motor

		ui_task();					//Tarea de control de la UI

		check_VBus_task();			//Tarea de monitoreo del VBus

		//heater_task();

		//sc_task();				//Tarea que monitorea la velocidad
	}
}