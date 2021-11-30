#include "AL03_heater_driver.h"


int32_t heater_actual_state = HEATER_CONFIG_STATE;
int32_t heater_previous_state;

int8_t heater_actual_pot;							//Variable que guarda la potencia actual usada 0-1-2-3

void heater_task (void)
{
	static uint32_t heater_timer;
	static uint8_t heater_tick = 0;
	static uint8_t b1_seq[HEATER_DEFAULT_PERIOD];		//Condicion base de rama 1
	static uint8_t b2_seq[HEATER_DEFAULT_PERIOD];		//Condicion base de rama 2
	static uint8_t aux_b1;
	static uint8_t aux_b2;
		
	switch (heater_actual_state)
	{
		case HEATER_CONFIG_STATE:

			heater_outputs_state_init();
			heater_actual_state = HEATER_LOAD_STATE;
			heater_timer = board_scheduler_load_timer(1500);
			
			break;


		case HEATER_LOAD_STATE:
			//Cargo los valores de las variables

			//heater_period_ticks = HEATER_DEFAULT_PERIOD; 					//Esta hay que reemplazarla para que lea de la tabla
			//heater_b1_active_ticks = (heater_period_ticks * duty1)/100;		//Esta hay que reemplazarla para que lea de la tabla
			//heater_b2_active_ticks = (heater_period_ticks * duty2)/100;		//Esta hay que reemplazarla para que lea de la tabla

			//Inicializo los vectores
			for (size_t i = 0; i < HEATER_DEFAULT_PERIOD; i++)
			{
				if( i%2 == 0)
				{
					b1_seq[i] = 0;
					b2_seq[i] = 1;
				}
				else
				{
					b1_seq[i] = 1;
					b2_seq[i] = 0;
				}
			}
			aux_b1 = BRANCH_OFF;
			aux_b2 = BRANCH_ON;

			heater_actual_state = HEATER_SWITCHING_STATE;

			break;


		case HEATER_SWITCHING_STATE:

			if (board_scheduler_is_time_expired(heater_timer))
			{
				if (heater_zcd_pin_get_state() == FALSE)
				{
					if(heater_tick >= HEATER_DEFAULT_PERIOD)
					{
						heater_tick = 0;
						heater_actual_state = HEATER_LOAD_STATE;
					}
					else
					{
						if(b1_seq[heater_tick] == 1)
						{
							heater_b1_set();
							aux_b1 = BRANCH_ON;
						}
						else
						{
							heater_b1_reset();
							aux_b1 = BRANCH_OFF;
						}

						if(b2_seq[heater_tick] == 1)
						{
							heater_b2_set();
							aux_b2 = BRANCH_ON;
						}
						else
						{
							heater_b2_reset();
							aux_b2 = BRANCH_OFF;
						}

						//Programo para la proxima lectura del pulso
						heater_timer = board_scheduler_load_timer(HEATER_PULSE_TON);
						heater_tick++;
					}
				}
			}

			break;
	}	
}




void load_seq( uint8_t* seq, uint8_t duty, uint8_t br_numb )
{
	//Funcion que calcula y carga los valores del vector con el duty especificado
	//Por defecto la rama se carga a un 50% y a eso se le suma o resta de acuerdo con el duty indicado
	//br_numb es el numero de rama que va a cargar, se usa para mejorar la separacion entre pulsos

	uint8_t i;
	if (duty > 95)
	{
		for (i = 0; i < HEATER_DEFAULT_PERIOD; i++)
		{
			seq[i] = 1;
		}
	}
	else if (duty < 5)
	{
		for (i = 0; i < HEATER_DEFAULT_PERIOD; i++)
		{
			seq[i] = 0;
		}
	}
	else
	{

	}
	
	
	
}



/*
case HEATER_SWITCHING_STATE:

			if (board_scheduler_is_time_expired(heater_timer))
			{
				if (heater_zcd_pin_get_state() == FALSE)
				{
					//Programo para la proxima lectura del pulso
					heater_timer = board_scheduler_load_timer(HEATER_PULSE_TON);
					heater_tick++;

					aux_calc = (heater_period_ticks-heater_tick);

					if(aux_calc <= 0)
					{
						heater_tick = 0;
						heater_actual_state = HEATER_LOAD_STATE;
					}
					else
					{
						//Realizo el control de la rama 1
						if ((aux_calc / heater_b1_active_ticks) >= 2)
						{
							//Continuo con la situacion de 1x1
							if (aux_b1 == BRANCH_ON)
							{
								heater_b1_reset();
								aux_b1 = BRANCH_OFF;
							}
							else
							{
								heater_b1_set();
								aux_b1 = BRANCH_ON;
							}
						}
						else
						{
							//Hago 2 pulsos en ON consecutivos
							heater_b1_set();
							aux_b1 = BRANCH_ON;
						}
						heater_b1_active_ticks--;
					}
				}
			}

			break;
*/

/*
case HEATER_CONFIG_STATE:

			heater_outputs_state_init();
			heater_actual_state = HEATER_WAITING_STATE;
			heater_timer = board_scheduler_load_timer(1000);
			
			break;

		case HEATER_TIME_MACHINE_STATE:
			//Estado que maneja el tiempo entre pulsos
			if (board_scheduler_is_time_expired(heater_timer))
			{
				if (heater_zcd_pin_get_state() == FALSE)
				{
					//Programo la proxima lectura en 15ms
					heater_timer = board_scheduler_load_timer(HEATER_PULSE_TOFF);

					heater_tick++;

					//Si se cumplio el periodo reinicio el contador
					heater_tick = heater_tick % heater_period_ticks;
				}
			}

			break


		case HEATER_OFF_STATE:
			
			if (board_scheduler_is_time_expired(heater_timer))
			{

			}

			break



		case HEATER_ON_STATE:
			static int8_t aux_pulso=0;		//Auxiliar para el pulso negativo (no se detecta con el zcd)
			if (board_scheduler_is_time_expired(heater_timer))
			{
				
			}

			//Cambio de estado a TON
			heater_actual_state = HEATER_ON_STATE;
			heater_timer = board_scheduler_load_timer(HEATER_PULSE_TON);

			break
*/

/*
					//Revisar si el NTC de temperatura calefa lo permite
					if ()
					{
						//Paso al estado ON

						
						//Rama 1
						if (heater_tick >= heater_ton_ticks)
						{
							//Estado Rama 1 en bajo
							heater_output_1_reset();
						}
						else
						{
							heater_output_1_set();
						}

						//Rama 2
						if (heater_tick >= heater_ton_ticks)
						{
							//Estado Rama 2 en alto
							heater_output_1_set();
						}
						else
						{
							heater_output_1_reset();
						}
					}
					else
					{
						//Si tengo sobretemperatura, mantengo apagados los calefa
					}
					*/