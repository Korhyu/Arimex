#include "AL03_speed_control.h"
#include "AL03_motor_3phase_driver.h"









void sc_task (void)
{
	uint32_t sc_timer;

	uint8_t  index = 0;
	int8_t	speed_control_state = SC_CONFIG_STATE;

	int32_t speed_vector[SC_VECTOR_SIZE];
	int32_t speed_avg;

	switch (speed_control_state)
	{
		case SC_CONFIG_STATE:
			sc_timer = SC_INITAL_WAIT;
			for (size_t i = 0; i < SC_VECTOR_SIZE; i++)
			{
				speed_vector[i] = 1700;		//TODO: Esto hay que cambiarlo mas adelante para que tome el valor de la velocidad dejada antes de apagarse
			}
			speed_avg = 1700;

			speed_control_state = SC_WORKING_STATE;
			
			break;

		case SC_WORKING_STATE:
			//Espero que se cumpla el tiempo y tomo una nueva muestra
			if(board_scheduler_is_time_expired(sc_timer))
			{
				//Tomo una muestra nueva
				speed_vector[index] = sc_sample();

				//Aumento el indice y lo reinicio si es necesario
				index++;
				index = index % SC_VECTOR_SIZE;

				if (index == 0)
				{
					//Puedo calcular el promedio y ver si tomo acciones
					speed_avg = sc_average(speed_vector);
					//Con el promedio puedo ver si tomo accion sobre el duty
					speed_control_state = SC_DECISION_STATE;
				}	
			}
			break;


		case SC_DECISION_STATE:
			//Tengo el promedio listo, lo evaluo contra el Set Point

			//TODO: Hay que ver que diferencia de velocidad se consigue con 1 punto de duty y con eso establecer los umbrales
			//Quizas esto sea responsabilidad del supervisor?



			speed_control_state = SC_WORKING_STATE;

			break;
		
		default:
			break;
	}
}


//Funcion que pide la velocidad del rotor actual para usar como muestra
int32_t sc_sample ()
{

}



//Promedia todos los valores del vector de muestas
int32_t	sc_average	(int32_t* speed_vector)
{
	int32_t	aux;
	for (size_t i = 0; i < SC_VECTOR_SIZE; i++)
	{
		aux += speed_vector[i];
	}

	aux = (aux >> 4);		//Este 4 es porque son 16 muestras, revisar si se puede hacer algo mas "dinamico"
	return aux;
}



//Verifica si VALUE esta dentro de REFERENE ± DELTA y devuelve TRUE o FALSE
int8_t sc_range_check (int32_t value, int32_t reference, int32_t delta)
{
	if ( value >= (reference-delta) )
	{
		if ( value <= (reference+delta) )
		{
			return SC_IN_RANGE;
		}
		else
		{
			return SC_ABOVE_RANGE;
		}
	}
	else
	{
		return SC_BELOW_RANGE;
	}	
}


//Devuelve el valor absoluto de variacion espedificada (expresada en "por miles") del paramereo enviado
//ARGUMENTO: Valor del cual calcula la variacion
//DELTA: Valor en x1000 para calcular la variacion
int32_t sc_delta_calc(int32_t argumento, int32_t delta)
{
	int32_t res;

	res = (argumento * delta)>>10;

	return res;
}