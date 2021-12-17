#include "AL03_vbus_control.h"
#include "AL04_ui_driver.h"
#include "AL03_motor_3phase_driver.h"


int8_t vbus_level;
int8_t vbus_actual_state = VBUS_CONFIG_STATE;

//Variable para el control de la velocidad
int8_t speed_control = VBUS_SPEED_CONTROL;

void check_VBus_task (void)
{
	static uint32_t vbus_timer = 1000;
	static uint16_t	vbus_sample = VBUS_HIGH_MIN_TH;
	static uint16_t	vbus_min = VBUS_HIGH_MIN_TH;
	static uint16_t sample_count = 0;
	 
	
	if (board_scheduler_is_time_expired(vbus_timer))
	{
		switch (vbus_actual_state)
		{
			case VBUS_CONFIG_STATE:		

				vbus_actual_state = VBUS_WAITING_STATE;
				vbus_timer = board_scheduler_load_timer(7);
				
				break;


			case VBUS_WAITING_STATE:
				//Cuando se cumple el tiempo tomo una nueva muestra
				vbus_sample = board_adc_get_vbus_voltage_C();
				sample_count++;

				if (vbus_min > vbus_sample)
				{
					vbus_min = vbus_sample;
				}
				

				if (sample_count > SAMPLE_WINDOW_SIZE )
				{
					if ( vbus_min < VBUS_VERY_LOW_MIN_TH)
					{
						//Guardo las variables
						vbus_level = VBUS_VERY_LOW_LEVEL;

						ui_led_slide(UI_HEATER, UI_UP);
						ui_led_slide(UI_HEATER, UI_UP);
						ui_led_slide(UI_HEATER, UI_UP);
					}
					else if (vbus_level == VBUS_VERY_LOW_LEVEL)
					{
						//Estaba en tension baja pero recupero
						//considerar este espacio para un re-arranque
						inverter_3phase_comm_set_seq(INVERTER_COMM_FREWHEEL, INVERTER_STATE_OVERWRITE);
					}

					//Control de velocidad usando la tension de linea
					if(speed_control == VBUS_SPEED_CONTROL && vbus_level != VBUS_VERY_LOW_LEVEL)
					{
						if ( vbus_min < VBUS_LOW_MIN_TH && vbus_level != VBUS_LOW_LEVEL)
						{
							//Paso a velocidad minima
							vbus_level = VBUS_LOW_LEVEL;

							motor_3phase_speed_change(LESS_SPEED);
							ui_led_slide(UI_FAN, UI_DOWN);
							motor_3phase_speed_change(LESS_SPEED);
							ui_led_slide(UI_FAN, UI_DOWN);

							//Si hay cambios, puedo esperar un buen tiempo antes de volver a tomar muestras
							vbus_timer = board_scheduler_load_timer(500);
						}
						else if ( vbus_min > VBUS_HIGH_MIN_TH && vbus_level != VBUS_HIGH_LEVEL)
						{
							//Paso a velocidad Maxima
							vbus_level = VBUS_HIGH_LEVEL;

							motor_3phase_speed_change(MORE_SPEED);
							ui_led_slide(UI_FAN, UI_UP);
							motor_3phase_speed_change(MORE_SPEED);
							ui_led_slide(UI_FAN, UI_UP);

							//Si hay cambios, puedo esperar un buen tiempo antes de volver a tomar muestras
							vbus_timer = board_scheduler_load_timer(500);
						}
					}

					//Reseteo las variables
					sample_count = 0;
					vbus_min = VBUS_HIGH_MIN_TH + 1;
				}
				else
				{
					//Si no realizo cambios, configuro para la proxima muestra
					vbus_timer = board_scheduler_load_timer(7);
				}

				

				break;
		}
	}	
}

/*
int16_t min_value_buffer (int16_t *buffer)
{
	int16_t min = buffer[0];
	int16_t index;

	for(index = 1; index < VBUS_SAMPLE_SIZE; index++)
	{
		if (min > buffer[index])
		{
			min = buffer[index];
		}
	}
	
	return min;
}

int16_t max_value_buffer (int16_t *buffer)
{
	int16_t max = buffer[0];
	int16_t index;

	for(index = 1; index < VBUS_SAMPLE_SIZE; index++)
	{
		if (max < buffer[index])
		{
			max = buffer[index];
		}
	}
	
	return max;
}

int16_t avg_value_buffer (int16_t *buffer)
{
	int16_t avg = buffer[0];
	int16_t index;

	for(index = 1; index < VBUS_SAMPLE_SIZE; index++)
	{
		avg += buffer[index];
	}
	
	return (avg / VBUS_SAMPLE_SIZE);
}
*/