/*
 * inverter_3phase.c
 *
 *  Created on: 30 jul. 2019
 *      Author: fgalli
 */

#include "AL02_inverter_3phase.h"

static int32_t inverter_actual_comm_seq=INVERTER_COMM_FREWHEEL;
static int32_t inverter_comm_direction=INVERTER_COMM_DIRECTION_FOWARD;

static void current_sensor_i_peak_callback (void);


/*******************************************************************************
 *
********************************************************************************/
int32_t inverter_3phase_init_config(void)
{
	//Cada vez que la corriente atraviese un umbral predeterminado se va a llamar a "current_sensor_i_peak_callback"
	board_comp_i_peak_disable_irq();
	board_comp_i_peak_link_callback(current_sensor_i_peak_callback);
	board_comp_i_peak_enable_irq();
	
	inverter_3phase_pwm_set_sense_on_toff_before_ton_us(10);
	
	inverter_3phase_comm_set_seq(INVERTER_COMM_FREWHEEL, INVERTER_STATE_OVERWRITE);
	//inicializar variables
	//etc
	return 0;
}

/*******************************************************************************
*  TRAPEZOIDAL COMMUTATION SIGNALS AND PHASES
*
*	EN SENTIDO INVERTER_COMM_DIRECTION_FOWARD
*
*    4|5|6|1|2|3|4|5|6|1|2|3|
*     | | |_|_| | | | |_|_| |
*  U _|_|/| | |\|_|_|/| | |\|
*    _| | | | |_|_| | | | |_|
*  V  |\|_|_|/| | |\|_|_|/| |
*     |_|_| | | | |_|_| | | |
*  W /| | |\|_|_|/| | |\|_|_|
*
*     	SEQ   Low   High  Comparator
*	1°   1     V     U       -W (negative slope)
*	2° 	 2     W     U        V (positive slope)
*	3°   3     W     V       -U
* 4°	 4     U     V        W
*	5° 	 5     U     W       -V
*	6°   6     V     W        U
*	7°	 1     V     U       -W
*	8°	 2     W     U        V
* 9°  ...   ...   ...      ...
*
*	   U = OUT1
*	   V = OUT2
*	   W = OUT3
*
*
*	EN SENTIDO INVERTER_COMM_DIRECTION_REVERSE
*
*	 	SEQ   Low   High  Comparator
*	1°   1     V     U        W
*	2° 	 6     V     W       -U
*	3°   5     U     W        V
* 4°	 4     U     V       -W
*	5° 	 3     W     V        U
*	6°   2     W     U       -V
*	7°	 1     V     U        W
*	8°	 6     V     W       -U
* 9°  ...   ...   ...      ...



*********************************************************************************/
int32_t inverter_3phase_comm_set_seq (int32_t inverter_comm_seq,int32_t inverter_state_overwrite)
{
	int32_t aux;
	

	if(inverter_state_overwrite == INVERTER_STATE_NOT_OVERWRITE)
	{
		aux = inverter_actual_comm_seq;		//Guardo el valor del estado
	}

	switch(inverter_comm_seq)
	{
		case INVERTER_COMM_SEQ1:
								inverter_actual_comm_seq = INVERTER_COMM_SEQ1;

								board_pwm_hin3_disable();		//OUT3: HIZ
								board_pwm_lin3_disable();

								board_pwm_lin1_disable();		//OUT1: PWM(VBUS)
								board_pwm_hin1_enable();

								board_pwm_hin2_disable();		//OUT2: GND
								board_pwm_lin2_enable();

								break;

		case INVERTER_COMM_SEQ2:
								inverter_actual_comm_seq = INVERTER_COMM_SEQ2;

								board_pwm_hin2_disable();		//OUT2: HIZ
								board_pwm_lin2_disable();

								board_pwm_lin1_disable();		//OUT1: PWM(VBUS)
								board_pwm_hin1_enable();

								board_pwm_hin3_disable();		//OUT3: GND
								board_pwm_lin3_enable();

								break;

		case INVERTER_COMM_SEQ3:
								inverter_actual_comm_seq = INVERTER_COMM_SEQ3;

								board_pwm_hin1_disable();
								board_pwm_lin1_disable();

								board_pwm_lin2_disable();
								board_pwm_hin2_enable();

								board_pwm_hin3_disable();
								board_pwm_lin3_enable();

								break;

		case INVERTER_COMM_SEQ4:
								inverter_actual_comm_seq = INVERTER_COMM_SEQ4;

								board_pwm_hin3_disable();
								board_pwm_lin3_disable();

								board_pwm_lin2_disable();
								board_pwm_hin2_enable();

								board_pwm_hin1_disable();
								board_pwm_lin1_enable();

								break;

		case INVERTER_COMM_SEQ5:
								inverter_actual_comm_seq = INVERTER_COMM_SEQ5;

								board_pwm_hin2_disable();
								board_pwm_lin2_disable();

								board_pwm_lin3_disable();
								board_pwm_hin3_enable();

								board_pwm_hin1_disable();
								board_pwm_lin1_enable();

								break;

		case INVERTER_COMM_SEQ6:
								inverter_actual_comm_seq = INVERTER_COMM_SEQ6;

								board_pwm_hin1_disable();
								board_pwm_lin1_disable();

								board_pwm_lin3_disable();
								board_pwm_hin3_enable();

								board_pwm_hin2_disable();
								board_pwm_lin2_enable();

								break;

		case INVERTER_COMM_FREWHEEL:
								inverter_actual_comm_seq = INVERTER_COMM_FREWHEEL;

								board_pwm_lin1_disable();
								board_pwm_lin2_disable();
								board_pwm_lin3_disable();
								board_pwm_hin1_disable();
								board_pwm_hin2_disable();
								board_pwm_hin3_disable();

								break;


		case INVERTER_COMM_BREAK_LOW:
								inverter_actual_comm_seq = INVERTER_COMM_BREAK_LOW;

								board_pwm_lin1_enable();
								board_pwm_lin2_enable();
								board_pwm_lin3_enable();
								board_pwm_hin1_disable();
								board_pwm_hin2_disable();
								board_pwm_hin3_disable();

								break;


		case INVERTER_COMM_BREAK_HIGH:
								inverter_actual_comm_seq = INVERTER_COMM_BREAK_HIGH;

								board_pwm_lin1_disable();
								board_pwm_lin2_disable();
								board_pwm_lin3_disable();
								board_pwm_hin1_enable();
								board_pwm_hin2_enable();
								board_pwm_hin3_enable();

								break;
		

		default:
							return -1;
	}

	if(inverter_state_overwrite == INVERTER_STATE_NOT_OVERWRITE)
	{
		inverter_actual_comm_seq = aux;
	}

	//PONER LOS CONTADORES DE PWM SEGUN LO QUE SE QUIERA HACER. EL PWM PRINCIPAL SETEARLO EN 0
	//EL PWM SECUNDARIO PARA PONER TOFFS SE PONE SEGUN LO QUE SE CONFIGURE
	board_pwm_start_with_ton();

	//__hardware_gpio_output_toggle(GPIOA, 3);					//GPIO aux para monitoreo en OSC
	return 0;
}



/*******************************************************************************
*	Acelerando la desmagnetizacion de la inductancia.
*
*	Descarga de inductancia de motor previo a pasar a una secuencia de sensado
*	de pendiente positiva.
*
*	En sentido FOWARD:
*
*	Estando en la SEQ5, un tiempo antes de pasar a SEQ6. Hay que abrir todos los transistores altos.
*	Y poner V y W a LOW
*
*	Estando en la SEQ3, un tiempo antes de pasar a SEQ4. Hay que abrir todos los transistores altos.
*	Y poner U y V a LOW
*
*	Estando en la SEQ1, un tiempo antes de pasar a SEQ2. Hay que abrir todos los transistores altos.
*	Y poner U y W a LOW
*
*
*	En sentido REVERSE:
*
*	Estando en la SEQ6, un tiempo antes de pasar a SEQ5. Hay que abrir todos los transistores altos.
*	Y poner U y W a LOW
*
*	Estando en la SEQ4, un tiempo antes de pasar a SEQ3. Hay que abrir todos los transistores altos.
*	Y poner V y W a LOW
*
*	Estando en la SEQ2, un tiempo antes de pasar a SEQ1. Hay que abrir todos los transistores altos.
*	Y poner V y U a LOW
*
*********************************************************************************/
void inverter_3phase_discharge_inductance_before_positive_slope_sense(void)
{
	inverter_3phase_comm_set_seq(INVERTER_COMM_FREWHEEL, INVERTER_STATE_NOT_OVERWRITE);

	if(inverter_comm_direction == INVERTER_COMM_DIRECTION_FOWARD)
	{
		switch(inverter_actual_comm_seq)
		{
			case INVERTER_COMM_SEQ1:	board_pwm_lin1_enable();
										board_pwm_lin3_enable();
										break;
			case INVERTER_COMM_SEQ3:	board_pwm_lin1_enable();
										board_pwm_lin2_enable();
										break;
			case INVERTER_COMM_SEQ5:	board_pwm_lin2_enable();
										board_pwm_lin3_enable();
										break;
		}
	}
	else
	{
		switch(inverter_actual_comm_seq)
		{
			case INVERTER_COMM_SEQ2:	board_pwm_lin1_enable();
										board_pwm_lin2_enable();
										break;
			case INVERTER_COMM_SEQ4:	board_pwm_lin2_enable();
										board_pwm_lin3_enable();
										break;
			case INVERTER_COMM_SEQ6:	board_pwm_lin1_enable();
										board_pwm_lin3_enable();
										break;
		}
	}
}





/*******************************************************************************
 *
********************************************************************************/
int32_t inverter_3phase_comm_next_seq (void)
{
	if(inverter_actual_comm_seq>=INVERTER_COMM_SEQ1 && inverter_actual_comm_seq<=INVERTER_COMM_SEQ6)
	{
		if(inverter_comm_direction == INVERTER_COMM_DIRECTION_FOWARD)
		{
			if(inverter_actual_comm_seq==INVERTER_COMM_SEQ6)
				inverter_actual_comm_seq = INVERTER_COMM_SEQ1;
			else
				inverter_actual_comm_seq<<=1;
		}
		else
		{
			if(inverter_actual_comm_seq==INVERTER_COMM_SEQ1)
				inverter_actual_comm_seq = INVERTER_COMM_SEQ6;
			else
				inverter_actual_comm_seq>>=1;
		}
		inverter_3phase_comm_set_seq(inverter_actual_comm_seq,INVERTER_STATE_OVERWRITE);
	}
	else
		return -1;
	return 0;
}


/*******************************************************************************
 *
********************************************************************************/
int32_t	inverter_3phase_set_comm_direction(int32_t inverter_direction)
{
	if(inverter_direction ==INVERTER_COMM_DIRECTION_FOWARD || inverter_direction ==INVERTER_COMM_DIRECTION_REVERSE )
		inverter_comm_direction = inverter_direction;
	else
		return -1;
	return 0;
}

/*******************************************************************************
 *
********************************************************************************/
int32_t	inverter_3phase_get_comm_direction(void)
{
	return inverter_comm_direction;
}

/*******************************************************************************
 *
********************************************************************************/
int32_t	inverter_3phase_get_actual_comm_seq (void)
{
	return inverter_actual_comm_seq;
}



/**************************************************************************************************************
 * Apaga los transistores de arriba justo luego de realizar un cambio de secuencia.
 * Si se conoce el periodo que tendra la secuencia, se puede configurar un tiempo de apagado antes que finalice la secuencia
 *
 * Si se desconoce el periodo en us debe ponerse 0 en periodo y en ese caso la funcion va a ignorar el parametro de "toff_us_final"
 *
 *
 * 		  ||<--------comm_period_us----------->||<--------comm_period_us----------->||
 *
 *     COMM					   			                COMM						            	       COMM
 * 	    ||               SEQ3        	 	     ||               SEQ4        	 	    ||
 * 		  ||   								   							 ||							 											||
 * 		  ||   TOFF    |      PWM      |  TOFF ||									                  ||
 * 		  ||   		      ____   ____   _        ||		         ____   ____   _        ||
 * 		  ||___________|    |_|    |_| |_______||___________|    |_|    |_| |_______||
 * 	 	   toff_begin_us             toff_final_us
 *
 ****************************************************************************************************************/

int32_t inverter_3phase_insert_toff_at_comm_begin_and_final(int32_t toff_us_begin,int32_t comm_period_us,int32_t toff_us_final)
{
	return 0;
}
/*******************************************************************************
 *
********************************************************************************/
int32_t inverter_3phase_get_toff_at_comm_begin(void)
{
	int32_t toff_begin=0;
	return toff_begin;
}
/*******************************************************************************
 *
********************************************************************************/
int32_t inverter_3phase_get_toff_at_comm_final(void)
{
	int32_t toff_final=0;
	return	toff_final;
}



/*******************************************************************************
 *
********************************************************************************/
int32_t inverter_3phase_get_actual_bemf_slope (void)
{
	if(inverter_actual_comm_seq&(INVERTER_COMM_SEQ2|INVERTER_COMM_SEQ4|INVERTER_COMM_SEQ6))
	{
		if(inverter_comm_direction==INVERTER_COMM_DIRECTION_FOWARD)
		{
			return INVERTER_BEMF_SLOPE_POSITIVE;
		}
		else
			return INVERTER_BEMF_SLOPE_NEGATIVE;
	}
	else
	{
		if(inverter_comm_direction==INVERTER_COMM_DIRECTION_FOWARD)
		{
			return INVERTER_BEMF_SLOPE_NEGATIVE;
		}
		else
			return INVERTER_BEMF_SLOPE_POSITIVE;
	}
}
/*******************************************************************************
 *
********************************************************************************/
int32_t inverter_3phase_get_actual_bemf_out (void)
{
	if(inverter_actual_comm_seq&(INVERTER_COMM_SEQ1|INVERTER_COMM_SEQ4))
		return INVERTER_BEMF_ON_OUTPUT_3;
	if(inverter_actual_comm_seq&(INVERTER_COMM_SEQ2|INVERTER_COMM_SEQ5))
		return INVERTER_BEMF_ON_OUTPUT_2;
	if(inverter_actual_comm_seq&(INVERTER_COMM_SEQ3|INVERTER_COMM_SEQ6))
		return INVERTER_BEMF_ON_OUTPUT_1;

	return -1;
}


/*******************************************************************************
 * Entra en esta funcion cada vez que la corriente por el shunt atraviese el umbral que tiene definido.
********************************************************************************/
void current_sensor_i_peak_callback (void)
{	
	//board_pwm_set_outputs_to_toff();							//Original de Fran
	//board_pwm_reset_counter();										//Alternativa 2 Jose
	board_pwm_set_counter_to_toff();								//Alternativa 1 Jose
}



