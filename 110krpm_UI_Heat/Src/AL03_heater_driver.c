#include "AL03_heater_driver.h"


int32_t heater_actual_state = HEATER_CONFIG_STATE;
int32_t heater_previous_state;

void heater_task (void)
{
	static uint32_t heater_timer = 0;
	
	
	if (board_scheduler_is_time_expired(heater_timer) || heater_timer==0)
	{
		if (heater_zcd_pin_get_state() == FALSE)
		{
			//Programo la proxima lectura en 15ms
			heater_timer = board_scheduler_load_timer(15);
		}
		
		

	}
	
	
	
	switch (heater_actual_state)
	{
		case HEATER_CONFIG_STATE:

			heater_outputs_state_init();
			heater_actual_state = HEATER_WAITING_STATE;
			heater_timer = board_scheduler_load_timer(1000);
			
			break;

		case HEATER_WAITING_STATE:

			break
	}
	
}

