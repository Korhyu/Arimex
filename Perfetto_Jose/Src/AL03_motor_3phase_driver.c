/*
 * AL03_motor_3phase_driver.c
 *
 *  Created on: 31 jul. 2019
 *      Author: fgalli
 */

#include "AL03_motor_3phase_driver.h"

/*********************************************************
*    Parametros que gobiernan el arranque del motor      *
**********************************************************/
/*Tiempo de carga de bootstrap (ojo que tambien es frenado electrico del motor)*/
#define MOTOR_CHARGE_BOOTSTRAP_TIME_mS						200
/*Tiempo de alineacion del rotor a posicion conocida*/
#define MOTOR_ALIGNMENT_TIME_mS 									100
#define MOTOR_ALIGNMENT_SEQ												INVERTER_COMM_SEQ2

/*Configuracion de PWM para aplicar la alineacion del rotor - Modo Current Limit Cycle by Cycle*/
#define PWM_IN_CYCLE_BY_CYCLE_PERIOD							6000
#define PWM_IN_CYCLE_BY_CYCLE_TOFF    						300

/*Cantidad de secuencias que se va a excitar al motor sensando ZCD en modo SAMPLE AT END TOFF*/
#define STARTING_FIRST_STEPS_FROM_STAND_COUNT			30

/*Configuracion de PWM que se va a usar desde la primer conmutacion */
#define PWM_STARTING_PERIOD_uS 										100
#define PWM_STARTING_TON_uS 	  						 	 		30

/*Este tiempo sirve para determinar cuanto blanking aplicar al primer paso que se da*/
#define START_FIRST_STEP_DURATION_INIT_us				  7000

/*Este blanking se usa cada vez que se pretende medir ZCD en freewheel. Depende del motor*/
#define DISABLE_ZCD_DETECTION_AFTER_FREEWHEEL_SETTING_us	200

#define MAX_TIMEOUT_WATCHDOG_us 						  		30000

/*Estos parametros de aca abajo dominan la rampa de aceleracion en el arranque*/
#define SET_POINT_PWM_TON_UPDATE_TIME_mS		  1		/*Intervalo en milisegundos en que se modifica el Ton del PWM*/
#define SET_POINT_MAX_PWM_TON_uS							88	/*Valor de TON maximo para la rampa (OJO NO PUEDE SER MENOR QUE EL PERIODO NI QUE "PWM_STARTING_TON_uS")*/
#define SET_POINT_MIN_PWM_TON_uS							20
#define SET_POINT_PWM_TON_INC_DEC_uS	 				1		/*Valor de incremento/decremento de TON */

#define TIME_TO_GET_RUNNING_TIMEOUT_mS				200


#define MASKED_SET_POINT_PWM_TON(set_point)		(set_point-(set_point&(SET_POINT_PWM_TON_INC_DEC_uS-1)))


/*Blanking que se aplica cuando esta en close loop y correct timming*/
#define bemf_close_loop_blanking_time_calc(comm_time_us) (((comm_time_us)>>3)+((comm_time_us)>>6))

/*Calculo por default que usa para situar ZCD en la conmutacion (modificar esto modifica el avance)*/
#define __default_zcd_expected_calc(time) ((time>>2)+(time>>4))
/*********************************************************
**********************************************************/



//******************************************
//Definiciones de uso privado en el archivo
//******************************************

//Estados de las maquina de estados del arranque del motor
#define STARTING_STATE_INIT			 										0
#define STARTING_STATE_BREAK_AND_CHARGE_BOOTSTRAP		1
#define STARTING_STATE_ALIGMENT											2
#define STARTING_STATE_FIRST_STEPS_FROM_STAND				3
#define STARTING_STATE_FREEWHEEL_PERIOD_MEASURE			4
#define STARTING_STATE_FREEWHEEL_SYNC								5
#define STARTING_STATE_STEPS_CORRECT_TIMMING				6
#define STARTING_STATE_COUNT												8

#define STARTING_SUB_STATE_UPDATING_PWM_SET_POINT		0
#define STARTING_SUB_STATE_ANALYZING_FAIL_SYNC			1
#define STARTING_SUB_STATE_CORRECTING_PARAMETERS		2
#define STARTING_SUB_STATE_RUNNING									3

//Estados de BEMF en closeloop "correct timming"
#define BEMF_STATE_COMMUTATE						0
#define BEMF_STATE_BLANKING_ZCD					1
#define BEMF_STATE_WAITING_ZCD					2
#define BEMF_STATE_ZCD_DETECTED					3
#define BEMF_STATE_ZCD_LOST							4

//Tipos de sensado de ZCD
#define BEMF_SENSE_TYPE_SAMPLE_END_TOFF			0
#define BEMF_SENSE_TYPE_CONTINUOUS					1


#define STOP_RUNNING_FLAG_STOP_MOTOR				0
#define STOP_RUNNING_FLAG_DONT_STOP_MOTOR		1

#define FAIL_SYNC_RATE_SENSE_DISABLE				0
#define FAIL_SYNC_RATE_SENSE_ENABLE					1

void motor_3phase_starting_state_machine		(void);
void bemf_zcd_disable_detection_within_time_us 	(int32_t time_us);
void bemf_sense_set_type						(int32_t bemf_sense_type_p);


int32_t  motor_3phase_abort_motor				(int32_t motor_stop_method,int32_t motor_state_exit_value);


void end_of_toff_pwm_callback					(void);
void blanking_timer_expired_callback	(void);
void bemf_comp_callback								(void);
void motor_watchdog_callback					(void);
void commutation_callback							(void);

void link_zcd_expected_calculate_function (int32_t(*func_pointer)(int32_t));

int32_t calculate_zcd_expected_default (int32_t time_zcd_to_zcd_avg);


//Variables globales privadas del archivo
//******************************************
int32_t (*calculate_zcd_expected)(int32_t);	//Puntero a la funcion que calcula ZCD expected

static volatile struct motor_3phase_drive gv; //Variables globales de este archivo




/*********************************************************************
 * Devuevel el maximo set point que se puede poner
**********************************************************************/
int32_t motor_3phase_get_pwm_ton_us_max_set_point(void)
{
	return SET_POINT_MAX_PWM_TON_uS;
}
/*********************************************************************
 * Devuevel el minimo set point que se puede poner
**********************************************************************/
int32_t motor_3phase_get_pwm_ton_us_min_set_point(void)
{
	return SET_POINT_MIN_PWM_TON_uS;
}

/*********************************************************************
 *	Esta funcion setea el valor de Ton del pwm que se desea aplicarse.
 *	El controlador del motor llegar a ese Ton indicado pero hará
 *	un barrido/rampa hacia ese valor gobernada por las siguientes definiciones
 *
 *	SET_POINT_PWM_TON_UPDATE_TIME_mS y SET_POINT_PWM_TON_INC_DEC_uS
 *
 *	Si se elije un Ton por fuera de los limites definidos por:
 *
 *	SET_POINT_MAX_PWM_TON_uS y SET_POINT_MAX_PWM_TON_uS
 *
 *	Asignara saturará al limite que corresponda y retornará -1.
 *
 *	En caso que el valor de Ton deseado esté comprendido dentro de los
 *	umbrales definidos, lo asignará al set point y retornará 0
 ********************************************************************/
int32_t motor_3phase_set_pwm_ton_us_set_point(int32_t pwm_ton_us_set_point)
{
	if(pwm_ton_us_set_point<=SET_POINT_MAX_PWM_TON_uS && pwm_ton_us_set_point>=SET_POINT_MIN_PWM_TON_uS)
	{
		gv.pwm_ton_us_set_point = pwm_ton_us_set_point;
		return 0;
	}
	else if(pwm_ton_us_set_point>SET_POINT_MAX_PWM_TON_uS)
	{
		gv.pwm_ton_us_set_point = SET_POINT_MAX_PWM_TON_uS;
		return -1;
	}
	else if(pwm_ton_us_set_point<SET_POINT_MIN_PWM_TON_uS)
	{
		gv.pwm_ton_us_set_point = SET_POINT_MIN_PWM_TON_uS;
		return -1;
	}
	return -2;
}

/*********************************************************************
 *  Esta funcion retorna el set point de TON del pwm aplicado en microsegundos
 *********************************************************************/
int32_t motor_3phase_get_pwm_ton_us_set_point(void)
{
	return gv.pwm_ton_us_set_point;
}


int32_t bemf_get_fail_sync_rate_ppm (void)
{
	return gv.fail_sync_ppm;
}


/*******************************************************************************
 *	Esta funcion asigna el funcion de giro, solo sera efectiva cuando el estado
 *	del motor sea STOPPED o FAIL. En ese caso retornara 0, sino retornara -1
 *	indicando que no fue efectiva la asignación del sentido de giro.
********************************************************************************/
int32_t motor_3phase_set_motor_direction(int32_t motor_direction)
{
	if(gv.motor_state==MOTOR_STATE_STOPPED || gv.motor_state==MOTOR_STATE_FAIL)
	{
		inverter_3phase_set_comm_direction(motor_direction);
		return 0;
	}
	return -1;
}



/*******************************************************************************
 *	Esta funcion retorna el valor de la maquina de estados del motor:
 *	MOTOR_STATE_STOPPED,MOTOR_STATE_STARTING,MOTOR_STATE_RUNNING,MOTOR_STATE_FAIL
********************************************************************************/
int32_t motor_3phase_get_state (void)
{
	return gv.motor_state;
}

/*******************************************************************************
 *	Esta funcion retorna la frecuencia electrica del motor (asociada a la rpm)
 *	En motores de 1 par   de polo  magnetico  en el rotor: RPM = f*60
 *	En motores de 2 pares de polos magneticos en el rotor: RPM = f*60/2
 *
 *	Solo retorna la frecuencia si el motor esta en estado RUNNING, sino retorna 0
********************************************************************************/
int32_t  motor_3phase_get_electrical_frequency_hz	(void)
{
	if(gv.starting_state<=STARTING_STATE_FIRST_STEPS_FROM_STAND)
		return 0;
	else
		return (int32_t)(1000000/gv.motor_electrical_period_us_avg);
}

#define bemf_blanking_timer_expired_callback_link(func_ptr)		board_timer_link_callback(func_ptr,BOARD_TIM_BLANKING_SEL_CALLBACK)
#define bemf_end_toff_sense_callback_link(func_ptr)						board_pwm_end_toff_link_callback(func_ptr)

#define bemf_sense_zcd_sampled_end_toff_enable()							board_pwm_break_enable_irq()
#define bemf_sense_zcd_sampled_end_toff_disable()							board_pwm_break_disable_irq()
#define bemf_sense_zcd_continous_enable()											board_comp_falling_edge_detection_only_enable(BOARD_COMP_BEMF)
#define bemf_sense_zcd_continuous_disable()										board_comp_bemf_disable_irqs_all_phases()

#define bemf_watchdog_timer_callback_link(func_ptr)						board_timer_link_callback(func_ptr,BOARD_TIM_WATCHDOG_SEL_CALLBACK)
#define bemf_watchdog_set_timeout_us(time_us)									board_tim_bftm_init_timer_with_timeout_irq_us(BOARD_TIM_BFTM_WATCHDOG,(time_us))
#define bemf_watchdog_get_count_us()													board_tim_get_bftm_count_us(BOARD_TIM_BFTM_WATCHDOG)
#define bemf_watchdog_abort()																	board_tim_bftm_abort(BOARD_TIM_BFTM_WATCHDOG)

#define bemf_commutation_set_within_us(time_us)								board_tim_bftm_init_timer_with_timeout_irq_us(BOARD_TIM_BFTM_COMM,(time_us))
#define bemf_commutation_get_count_us()												board_tim_get_bftm_count_us(BOARD_TIM_BFTM_COMM)
#define bemf_commutation_abort()															board_tim_bftm_abort(BOARD_TIM_BFTM_COMM)
#define bemf_commutation_callback_link(func_ptr)							board_timer_link_callback(func_ptr,BOARD_TIM_COMM_SEL_CALLBACK)

/*******************************************************************************
 *	Se llama una sola vez en el inicio del sistema, linkea callbacks
 *	y llama a configuraciones de hardware de mas bajo nivel (inverter trifasico)
 *	Tambien inicializa la maquina de estados del motor en el estado inicial
********************************************************************************/
int32_t motor_3phase_init(void)
{
	inverter_3phase_init_config();

	motor_3phase_set_motor_direction(MOTOR_DIRECTION_FOWARD);

	bemf_blanking_timer_expired_callback_link(blanking_timer_expired_callback);

	bemf_sense_zcd_sampled_end_toff_disable();		//Antes de linkear los callbacks, me aseguro de deshabilitar las detecciones
	bemf_sense_zcd_continuous_disable();					//Porque sino hay falsas llamadas

	bemf_end_toff_sense_callback_link(end_of_toff_pwm_callback);
	
	//board_comp_bemf_link_callback(bemf_comp_callback); - Hay que linkear las 3 irqs al mismo callback.
	board_comp_bemf_rising_link_callback(bemf_comp_callback,BOARD_COMP_BEMF_U_PHASE_SELECT);
	board_comp_bemf_rising_link_callback(bemf_comp_callback,BOARD_COMP_BEMF_V_PHASE_SELECT);
	board_comp_bemf_rising_link_callback(bemf_comp_callback,BOARD_COMP_BEMF_W_PHASE_SELECT);
	

	bemf_watchdog_timer_callback_link(motor_watchdog_callback);

	bemf_commutation_callback_link(commutation_callback);

	gv.motor_state = MOTOR_STATE_STOPPED;

	link_zcd_expected_calculate_function(calculate_zcd_expected_default);

	motor_3phase_set_pwm_ton_us_set_point(100);

	return 0;
}


/*******************************************************************************
 *	Esta funcion hay que estar llamandola constantemente en el while(1) del main
 *	Probablemente este siendo llamada por otra funcion un nivel mas arriba
********************************************************************************/
void motor_3phase_task(void)
{
	switch(gv.motor_state)
	{
		case MOTOR_STATE_STOPPED:
									break;

		case MOTOR_STATE_STARTING:
			//Jose: Falta codigo aca(?)
			//Falta la parte del motor funcionando normalmente (?)
			//Quizas este es el punto donde Fran "tiro la toalla"
		case MOTOR_STATE_RUNNING:
									motor_3phase_starting_state_machine();
									break;

		case MOTOR_STATE_FAIL:
									break;

		default:					while(1);//DEV_ERROR
	}
}



/*******************************************************************************
 *	Esta funcion se llama para iniciar la maquina de estados del motor
 *	y sacarla de los estados estacionarios FAIL y STOPPED
 *
 *	Si el motor estaba en STARTING o RUNNING no hace nada y devuelve -1, sino
 *	devuelve 0.
********************************************************************************/
int32_t motor_3phase_start_motor(void)
{
	if(gv.motor_state != MOTOR_STATE_STARTING && gv.motor_state != MOTOR_STATE_RUNNING)
	{
		gv.motor_state 		 = MOTOR_STATE_STARTING;
		gv.starting_state 	 = STARTING_STATE_INIT;
		gv.stop_running_flag = STOP_RUNNING_FLAG_DONT_STOP_MOTOR;
	}
	else
		return -1;

	return 0;
}


/*******************************************************************************
 *	Esta funcion se llama para detener la maquina de estados del motor
 *	y llevarla a los estados estacionarios FAIL y STOPPED y que sea mediante
 *	un frenado o dejando libre el rotor.
********************************************************************************/
int32_t  motor_3phase_abort_motor	(int32_t motor_stop_method,int32_t motor_state_exit_value)
{
	bemf_commutation_abort();
	bemf_sense_zcd_continuous_disable();
	bemf_sense_zcd_sampled_end_toff_disable();
	bemf_watchdog_abort();
	gv.stop_running_flag = STOP_RUNNING_FLAG_STOP_MOTOR;

	if(motor_stop_method==MOTOR_STOP_METHOD_FREEWHEEL)
	{
		inverter_3phase_comm_set_seq(INVERTER_COMM_FREWHEEL, INVERTER_STATE_OVERWRITE);
	}
	else
	{
		inverter_3phase_comm_set_seq(INVERTER_COMM_BREAK_LOW, INVERTER_STATE_OVERWRITE);
	}


	if	(motor_state_exit_value == MOTOR_STATE_STOPPED)
	{
		gv.motor_state = MOTOR_STATE_STOPPED;
		return 0;
	}
	else if (motor_state_exit_value == MOTOR_STATE_FAIL)
	{
		gv.motor_state = MOTOR_STATE_FAIL;
		return 0;
	}
	else
		gv.motor_state = MOTOR_STATE_FAIL;


	return -1;
}






int32_t  motor_3phase_stop_motor (int32_t motor_stop_method)
{
	if(motor_stop_method==MOTOR_STOP_METHOD_BREAK || motor_stop_method==MOTOR_STOP_METHOD_FREEWHEEL)
	{
		motor_3phase_abort_motor	(motor_stop_method,MOTOR_STATE_STOPPED);
		return 0;
	}
	else
	{
		return -1;
	}

}







/*******************************************************************************
 * Maquina de estados para el arranque del motor, estas instrucciones se ejecutan
 * en contexto normal, hay parte de la maquina de estados que cambiará en
 * contexto de interrupcion a partir de los llamados a zcd_event()
********************************************************************************/
void motor_3phase_starting_state_machine(void)
{
	static int32_t timer_bootstrap,timer_alineacion,timer_ramp,timer_to_running;

	switch (gv.starting_state)
	{
		case STARTING_STATE_INIT:
									timer_bootstrap = 0;
									timer_alineacion =0;
									gv.starting_state = STARTING_STATE_BREAK_AND_CHARGE_BOOTSTRAP;
									gv.fail_sync_rate_comm_counter=0;
									gv.fail_sync_rate_fails_counter=0;
									gv.fail_sync_ppm=0;
									break;

		case STARTING_STATE_BREAK_AND_CHARGE_BOOTSTRAP:
									if(timer_bootstrap == 0)
									{
										timer_bootstrap = board_scheduler_load_timer(MOTOR_CHARGE_BOOTSTRAP_TIME_mS);
										inverter_3phase_comm_set_seq(INVERTER_COMM_BREAK_LOW, INVERTER_STATE_OVERWRITE);
									}
									else if(board_scheduler_is_time_expired(timer_bootstrap))
									{
										gv.starting_state = STARTING_STATE_ALIGMENT;
									}
									break;

		case STARTING_STATE_ALIGMENT:
									if(timer_alineacion==0)
									{
										board_bemf_attenuation_disable();
										board_bemf_vref_select_vbus();

										//Pongo el PWM a funcionar como limite de corriente ciclo a ciclo.
										//Un periodo muy largo y un duty practicamente de 100%.
										//Lo importante es que el tiempo de apagado toff es 300uS. Esto signifca
										//Que una vez que la corriente alcanza el umbral, se va a apagar 300uS
										//y luego volvera a conducir hasta que la corriente vuelva a alcanzar el umbral.

										//Hay que setear siempre primero el periodo y despues Ton o Toff. Sino: puede fallar
										inverter_3phase_pwm_set_period_us(PWM_IN_CYCLE_BY_CYCLE_PERIOD);
										inverter_3phase_pwm_set_toff_us(PWM_IN_CYCLE_BY_CYCLE_TOFF);

										timer_alineacion = board_scheduler_load_timer(MOTOR_ALIGNMENT_TIME_mS);
										inverter_3phase_comm_set_seq(MOTOR_ALIGNMENT_SEQ, INVERTER_STATE_OVERWRITE);
									}
									else if(board_scheduler_is_time_expired(timer_alineacion))
									{
										gv.starting_state = STARTING_STATE_FIRST_STEPS_FROM_STAND;

										inverter_3phase_comm_next_seq();

										gv.starting_first_steps_counter=STARTING_FIRST_STEPS_FROM_STAND_COUNT;

										//Hay que setear siempre primero el periodo y despues Ton o Toff. Sino: puede fallar
										inverter_3phase_pwm_set_period_us(PWM_STARTING_PERIOD_uS);
										inverter_3phase_pwm_set_ton_us(PWM_STARTING_TON_uS);

										bemf_sense_set_type(BEMF_SENSE_TYPE_SAMPLE_END_TOFF);

										gv.motor_comm_seq_period_us_avg = START_FIRST_STEP_DURATION_INIT_us;

										bemf_watchdog_set_timeout_us(START_FIRST_STEP_DURATION_INIT_us<<2);

										bemf_zcd_disable_detection_within_time_us(START_FIRST_STEP_DURATION_INIT_us>>1);

									}
									break;

		case STARTING_STATE_FIRST_STEPS_FROM_STAND:
										gv.starting_sub_state = STARTING_SUB_STATE_UPDATING_PWM_SET_POINT;
										timer_ramp=0;
										timer_to_running = board_scheduler_load_timer(TIME_TO_GET_RUNNING_TIMEOUT_mS);
									break;

		case STARTING_STATE_FREEWHEEL_SYNC:
		case STARTING_STATE_FREEWHEEL_PERIOD_MEASURE:
		case STARTING_STATE_STEPS_CORRECT_TIMMING:

									switch(gv.starting_sub_state)
									{
										case STARTING_SUB_STATE_UPDATING_PWM_SET_POINT:
																		if(timer_ramp==0 || board_scheduler_is_time_expired(timer_ramp))
																		{
																			timer_ramp = board_scheduler_load_timer(SET_POINT_PWM_TON_UPDATE_TIME_mS);

																			if(inverter_3phase_pwm_get_ton_us()<MASKED_SET_POINT_PWM_TON(gv.pwm_ton_us_set_point))
																			{
																				inverter_3phase_pwm_set_ton_us(inverter_3phase_pwm_get_ton_us()+SET_POINT_PWM_TON_INC_DEC_uS);
																			}
																			else if (inverter_3phase_pwm_get_ton_us()>MASKED_SET_POINT_PWM_TON(gv.pwm_ton_us_set_point))
																			{
																				inverter_3phase_pwm_set_ton_us(inverter_3phase_pwm_get_ton_us()-SET_POINT_PWM_TON_INC_DEC_uS);
																			}
																			else
																			{
																				gv.motor_state = MOTOR_STATE_RUNNING;
																			}
																		}
																		break;

										case STARTING_SUB_STATE_ANALYZING_FAIL_SYNC:

																		break;

										case STARTING_SUB_STATE_CORRECTING_PARAMETERS:

																		break;

										case STARTING_SUB_STATE_RUNNING:
																			//En running hay que verificar que el set point es igual o no al actual,
																			//Cuando no sea igual hay que ir "STARTING_SUB_STATE_UPDATING_PWM_SET_POINT"
																		break;

										default: while(1);//DEV_ERROR
									}

									if(gv.motor_state != MOTOR_STATE_RUNNING && board_scheduler_is_time_expired(timer_to_running))
										motor_3phase_abort_motor(MOTOR_STOP_METHOD_BREAK,MOTOR_STATE_FAIL);


									break;


		default:					while(1);//DEV_ERROR
	}
}







/*******************************************************************************
*	Esta funcion se llama desde ZCD EVENT en los primeros pasos desde que el
*	motor se encuentra detenido.
*
*	Se encarga de realizar el cambio de secuencia de excitacion inmediatamente
*	cuando detecta ZCD.
*
*	Tambien mide el tiempo entre dos ZCD de pendiente positiva. (2 secuencias
*	de excitacion).
*
*	Pasada la cantidad de pasos fijada por "starting_first_steps_counter"
*	Deja el motor girar libre, mide el tiempo efectivamente y comienza la excitacion
*	nuevamente sobre ZCD detectado con el motor girando libre.
*
*	Se ejecuta en contexto de interrupcion ya que se llama desde "zcd_event()"
********************************************************************************/
#define AVG_FACTOR_ELECTRICAL_PERIOD	1
void zcd_event_first_steps_state (void)
{
	gv.bemf_state = BEMF_STATE_ZCD_DETECTED;

	//Mido el periodo entre dos detecciones de pendiente positiva. Esto es porque en algunos motores hay asimetria
	//en las duraciones de los periodos entre Pendiente positiva y Negativa.
	if(inverter_3phase_get_actual_bemf_slope() == INVERTER_BEMF_SLOPE_POSITIVE)
	{
		common_update_average(gv.motor_comm_seq_period_us_avg,bemf_watchdog_get_count_us(),AVG_FACTOR_ELECTRICAL_PERIOD);
		bemf_watchdog_set_timeout_us((gv.motor_comm_seq_period_us_avg<<2));
	}


	if(gv.starting_first_steps_counter>1)
	{
		if(inverter_3phase_get_actual_bemf_slope() == INVERTER_BEMF_SLOPE_NEGATIVE)
			gv.starting_first_steps_counter--;

		inverter_3phase_comm_next_seq();
		//Hago un blanking de ZCD de 1/8 de lo que dura una secuencia de excitacion.
		//En esta etapa el tiempo que se mide es el de dos secuencias de exctacion (de pendiente positiva a pendiente positiva)
		//Por eso primero se divide por 2, y luego se divide por 8.
		bemf_zcd_disable_detection_within_time_us((gv.motor_comm_seq_period_us_avg>>1)>>3);
	}


	if(gv.starting_first_steps_counter==1)
	{
		//Aca va a entrar cuando starting_first_steps_counter llegue a 1. Notar que ademas va a ser en el ZCD de pendiente Positiva
		// Con lo cual se hará lo siguiente.
		// Se dejará girar libre al motor para medir el periodo.

		//Estas  instrucciones de abajo dejaran al motor girando libre y preparan deteccion de ZCD girando libre
		//En una secuencia de pendiente POSITIVA.

		gv.starting_state = STARTING_STATE_FREEWHEEL_SYNC;

		board_bemf_vref_select_neutral_point();
		board_bemf_attenuation_enable();

		bemf_sense_set_type(BEMF_SENSE_TYPE_CONTINUOUS);

		inverter_3phase_comm_set_seq(INVERTER_COMM_FREWHEEL, INVERTER_STATE_NOT_OVERWRITE);

		bemf_zcd_disable_detection_within_time_us(DISABLE_ZCD_DETECTION_AFTER_FREEWHEEL_SETTING_us);
	}
}


/*******************************************************************************
*	Esta funcion se llama desde ZCD EVENT luego de haber dado los primeros
*	pasos determinados por "starting_first_steps_counter"
*
*	En esta rutina se mide el periodo de dos secuencias de excitacion pero
*	con el motor girando libre (freewheel). Luego deja todo listo para sincronizarse
*	en ZCD y comenzar a realizar la connmutacion de secuencias con el timming correcto
*
*	Se ejecuta en contexto de interrupcion ya que se llama desde "zcd_event()"
********************************************************************************/
void zcd_event_freewheel_period_measure(void)
{
	gv.bemf_state = BEMF_STATE_ZCD_DETECTED;

	if(gv.starting_state == STARTING_STATE_FREEWHEEL_SYNC)
	{

		bemf_watchdog_set_timeout_us(MAX_TIMEOUT_WATCHDOG_us); //Inicio el timer para medir periodo
		inverter_3phase_comm_next_seq();
		inverter_3phase_comm_next_seq();	 //Voy a medir el tiempo entre 2 ZCD de pendiente POSITIVA
		bemf_zcd_disable_detection_within_time_us(gv.motor_comm_seq_period_us_avg>>2);

		inverter_3phase_comm_set_seq(INVERTER_COMM_FREWHEEL, INVERTER_STATE_NOT_OVERWRITE);
		gv.starting_state = STARTING_STATE_FREEWHEEL_PERIOD_MEASURE;
	}
	else if(gv.starting_state == STARTING_STATE_FREEWHEEL_PERIOD_MEASURE)
	{

		//Estas inicializaciones de abajo son el "reset de variables" para que funcione bien CONTROL LOOP CALCULATE
		gv.motor_comm_seq_period_us_avg = bemf_watchdog_get_count_us(); //Es el tiempo entre dos ZCD de pendiente positiva
																		//Osea es el tiempo de 2 secuencias de excitacion
		gv.time_from_zcd_to_zcd = gv.motor_comm_seq_period_us_avg;

		gv.time_from_zcd_to_zcd_avg = gv.time_from_zcd_to_zcd;			//Esta variable se usa en el CONTROL LOOP CALCULATE

		gv.motor_comm_seq_period_us_avg>>=1;							//Correccion de la medicion para que corresponda a 1 periodo de secuencia.

		gv.motor_comm_seq_period_us = gv.motor_comm_seq_period_us_avg;

		gv.time_from_comm_to_zcd_avg = gv.motor_comm_seq_period_us_avg>>1;

		board_bemf_vref_select_vbus();

		bemf_commutation_set_within_us(((gv.motor_comm_seq_period_us_avg>>1)-11)); //Temporizo la conmutacion para hacerla dentro de medio
																				   //Periodo de secuencia de excitacion. Ya que esto se ejecuta
																				   //Cuando se está en ZCD. y ZCD debe caer en el medio (aprox)
																				   //De la secuencia de conmutacion

																				   //El "-11" es para corregir la temporizacion dado el tiempo
																				   //que demora en ejecutarse instrucciones por el uC
		bemf_watchdog_set_timeout_us(MAX_TIMEOUT_WATCHDOG_us);

		gv.starting_state = STARTING_STATE_STEPS_CORRECT_TIMMING;
	}
}



/*******************************************************************************
*	Se toda vez que el control ya sincronizado con el motor (habiendose sincronizado
*	con el motor girando libre) comenzo a realizar el lazo de control para calcular
*	los tiempos y temporizar correctamente las conmutaciones.
*
*	Se ejecuta en contexto de interrupcion ya que se llama desde "zcd_event()"
********************************************************************************/
void zcd_event_correct_timing_comm(void)
{
	gv.time_from_comm_to_zcd = bemf_commutation_get_count_us();

	gv.time_from_zcd_to_zcd = bemf_watchdog_get_count_us();
	bemf_watchdog_set_timeout_us((gv.motor_comm_seq_period_us_avg<<3));

	gv.bemf_state = BEMF_STATE_ZCD_DETECTED;

	bemf_sense_zcd_continuous_disable();
}


/*******************************************************************************
*	Se llega a esta funcion cuando se detecta ZCD.
*	En el modo BEMF_SENSE_TYPE_SAMPLE_END_TOFF, ZCD puede ser
*	detectada en la pendiente positiva y negativa de BEMF (es decir en todas
*	las secuencias de excitacion).
*
*	Mientras que en el modo BEMF_SENSE_TYPE_CONTINUOUS
*	Solo puede ser detectada por el comparador cuando la pendiente es positiva
*	(es decir solo en las secuencias SEQ2,SEQ4 y SEQ6)
*
*	Como se llama desde la IRQ del comparador o la del flanco del pin (end toff)
*
*	Esta funcion se ejecuta en contexto de interrupcion
********************************************************************************/
void zcd_event (void)
{
	if(gv.stop_running_flag != STOP_RUNNING_FLAG_STOP_MOTOR)
	{
		if(gv.starting_state == STARTING_STATE_FIRST_STEPS_FROM_STAND)
		{
			zcd_event_first_steps_state();
		}
		else if(gv.starting_state==STARTING_STATE_FREEWHEEL_PERIOD_MEASURE || gv.starting_state==STARTING_STATE_FREEWHEEL_SYNC)
		{
			zcd_event_freewheel_period_measure();
		}
		else if(gv.starting_state==STARTING_STATE_STEPS_CORRECT_TIMMING)
		{
			zcd_event_correct_timing_comm();
		}
	}
}






/********************************************************************************
 *  Esta funcion sirve para linkear nuevas funciones de calculo de
 *  ZCD_EXPECTED.
 *
 *  Es muy util cuando se quiera cambiar desde la ejecucion del programa
 *  el avance o retroceso de la conmutacion respecto de ZCD.
 *******************************************************************************/
void link_zcd_expected_calculate_function (int32_t(*func_pointer)(int32_t))
{
	calculate_zcd_expected = func_pointer;
}

/********************************************************************************
* Funcion por defecto para calcular ZCD Expected.
* Hace situar ZCD retrasada levemente de la mitad de la secuencia de excitacion
*
* Si se quisiera que ZCD esté siempre en la mitad de la secuencia de excitacion
* Deberia ponerse "return (time_zcd_to_zcd_avg>>2);"
*
* Aca como tiene un termino extra, ZCD queda "retrasado".
*
* Esto en verdad significa un AVANCE en la conmutacion de la secuencia.
*
* Mientras mas cerca esté la conmutación de ZCD, más adelanto se estará cometiendo
* en la conmutación.
*
* Esta funcion se ejecutara en contexto de interrupcion.
 *******************************************************************************/
int32_t calculate_zcd_expected_default (int32_t time_zcd_to_zcd_avg)
{
	return	__default_zcd_expected_calc(time_zcd_to_zcd_avg);
}

/*******************************************************************************
 *	Esta funcion calcula la temporizacion de cada secuencia de excitacion.
 *	Manteniendo la velocidad que deba tener el motor.
 *
 *	Los ajustes de avance o retroceso de la conmutacion se tocan en esta funcion
 *	en el calculo de time_zcd_expected.
 *******************************************************************************/
void bemf_control_loop_calculate(void)
{
	common_update_average(gv.time_from_zcd_to_zcd_avg, gv.time_from_zcd_to_zcd , 1); //Actualizo ZCD-ZCD_AVG

	common_update_average(gv.time_from_comm_to_zcd_avg,gv.time_from_comm_to_zcd,1);

	//Se espera que ZCD caiga en el medio de la secuencia. Osea que T_comm_seq = Tzcd-zcd/2
	//Para que caiga en la mitad de Tcomm_seq.
	//Tcomm_zcd_Expected = Tcomm_seq/2 = Tzcd_zcd/4. = Tzcd_zcd>>2
	//Se le agrega un pequeño ajuste, en funcion del adelanto o atraso que se le quiera meter a la conmutacion

	gv.time_zcd_expected = calculate_zcd_expected(gv.time_from_zcd_to_zcd_avg);

	//Voy a calcular cuanto voy a acortar la siguiente secuencia (que es sin sensar, en flanco negativo).
	//La correccion se debe a:
	//1) cuanto "de más" o "de menos" llego a conducir la ultima secuencia de flanco positivo. Dado que el periodo estaba precargado antes de
	//	 llegar a sensar ZCD y saber el periodo

	//2) Corrimientos en ZCD dado "ruido" (pwm-etc).

	gv.time_t_error = ((gv.time_from_zcd_to_zcd_avg>>1) - gv.motor_comm_seq_period_us);

	gv.time_t_error = gv.time_t_error + (gv.time_from_comm_to_zcd_avg - gv.time_zcd_expected);

	gv.motor_comm_seq_period_us = gv.time_from_zcd_to_zcd_avg>>1;

	bemf_commutation_set_within_us(gv.motor_comm_seq_period_us+gv.time_t_error);

	gv.motor_electrical_period_us_avg = (gv.motor_comm_seq_period_us<<2)+(gv.motor_comm_seq_period_us<<1);

}


/*******************************************************************************
*	Esta funcion se utiliza (es llamada) una vez que se pasa el primer modo de
*	de arranque (que sensa ZCD tipo SAMPLE AT END TOFF)
*
*	Y se encarga de habilitar el sensado de ZCD solo cuando va a ser Positiva
*	la pendiente de la BEMF; y cuando sea negativa hara calculos correctivos
*	para la temporizacion de las conmutaciones de las secuencias de excitacion
*
*	Tambien se encarga de sensar el FAIL_SYNC_RATE que indica cuantos fallos
*	hubo en detectar ZCD en una cantidad de conutaciones dadas.
 *******************************************************************************/
void commutation_callback(void)
{
	if(gv.stop_running_flag != STOP_RUNNING_FLAG_STOP_MOTOR)
	{
		inverter_3phase_comm_next_seq();

		//En el inicio de la nueva secuencia me fijo si la secuencia actual va a ser de pendiente
		//positiva o negativa de BEMF

		//En la pendiente positiva voy a setear la temporizacion para conmutacion de secuencia
		//y a habilitar el sensado ZCD despues del blanking

		//En la pendiente negativa voy a hacer los calculos para readaptar los tiempos de secuencia para que sigan al motor.
		//y voy a setear la temporizacion para la conmutacion de secuencia y deshabilitar el sensado de ZCD.

		if(inverter_3phase_get_actual_bemf_slope() == INVERTER_BEMF_SLOPE_POSITIVE)
		{//BEMF POSITIVE SLOPE
			gv.bemf_state = BEMF_STATE_WAITING_ZCD;
			bemf_commutation_set_within_us(gv.motor_comm_seq_period_us - 20);
			bemf_zcd_disable_detection_within_time_us(bemf_close_loop_blanking_time_calc(gv.motor_comm_seq_period_us_avg));
		}
		else if(inverter_3phase_get_actual_bemf_slope() == INVERTER_BEMF_SLOPE_NEGATIVE)
		{//BEMF NEGATIVE SLOPE

			if(gv.bemf_state != BEMF_STATE_ZCD_DETECTED)
			{//ZCD LOST!!
				//Si se pierde la deteccion, se vuelve a FREEWHEEL a detectar ZCD y volver a medir el periodo electrico de una secuencia.
				gv.bemf_state = BEMF_STATE_ZCD_LOST;
				inverter_3phase_comm_next_seq();
				inverter_3phase_comm_set_seq(INVERTER_COMM_FREWHEEL, INVERTER_STATE_NOT_OVERWRITE);
				board_bemf_vref_select_neutral_point();
				gv.starting_state = STARTING_STATE_FREEWHEEL_SYNC;
				bemf_zcd_disable_detection_within_time_us(gv.motor_comm_seq_period_us_avg>>1);
			}
			else
			{//ZCD DETECTED
				gv.bemf_state = BEMF_STATE_COMMUTATE;
				bemf_control_loop_calculate();
				bemf_sense_zcd_continuous_disable();
			}


			if(gv.fail_sync_rate_enable_flag == FAIL_SYNC_RATE_SENSE_ENABLE)
			{
				if(gv.bemf_state == BEMF_STATE_ZCD_LOST)
					gv.fail_sync_rate_fails_counter++;

				if(gv.fail_sync_rate_comm_counter<100)
					gv.fail_sync_rate_comm_counter++;
				else
				{
					//Multiplico las cuentas que hice por un numero cercano a 100mil para pasar a ppm
					gv.fail_sync_ppm = (gv.fail_sync_ppm +(gv.fail_sync_rate_fails_counter<<13)+(gv.fail_sync_rate_fails_counter<<11)-(gv.fail_sync_rate_fails_counter<<8))>>1;
					gv.fail_sync_rate_fails_counter=0;
					gv.fail_sync_rate_comm_counter=0;
				}
			}
		}//END BEMF_NEGATIVE SLOPE

	}
}




/*******************************************************************************
 *	Esta funcion carga una variable global de tipo de sensado ZCD
 *	En funcion de que parte del arranque del motor este, puede adoptar estos
 *	valores
 *
 *  BEMF_SENSE_TYPE_SAMPLE_END_TOFF
 * 	BEMF_SENSE_TYPE_CONTINUOUS
 *
 * 	La primera sirve para sensar ZCD desde el motor apagado para obtener
 * 	feedback del motor desde el primer cambio de secuencia de excitacion
********************************************************************************/
void bemf_sense_set_type(int32_t bemf_sense_type_p)
{
	gv.bemf_sense_type = bemf_sense_type_p;
}




/*******************************************************************************
 * Esta funcion se llama cuando esta seleccionado el sensado de ZCD
 *
 * BEMF_SENSE_TYPE_SAMPLE_END_TOFF
 *
 * Al final de cada tiempo de Toff del PWM se llama a esta funcion y evalua
 * el estado del comparador, en funcion de la secuencia en la que se encuentra
 * determinará si ya pasó ZCD o aún no.
 *
 * Se llama a esta funcion cada vez que estando la deteccion habilitada tipo SAMPLE
 * se genera un flanco o interrupcion un tiempo antes (determinado por "time_before_ton_us")
 * en el archivo del "inverter_3phase".
 *
 * Es un sensado del tipo SAMPLE (o MUESTREO PUNTUAL) ya que hace una unica
 * evaluacion al final del tiempo de apagado.
 *
 * Se ejecuta en contexto de interrupcion
********************************************************************************/
#define COMP_BEMF_OUT_SENSING_BEMF_IS_ABOVE_VREF BOARD_COMP_BEMF_IS_ABOVE_VREF
#define COMP_BEMF_OUT_SENSING_BEMF_IS_BELOW_VREF BOARD_COMP_BEMF_IS_BELOW_VREF


void end_of_toff_pwm_callback(void)
{
	int32_t aux;
	switch(inverter_3phase_get_actual_bemf_out())
	{
		case	INVERTER_BEMF_ON_OUTPUT_1:		aux = board_comp_bemf_get_output_u_phase();
																				break;
		case	INVERTER_BEMF_ON_OUTPUT_2:		aux = board_comp_bemf_get_output_v_phase();
																				break;
		case	INVERTER_BEMF_ON_OUTPUT_3:		aux = board_comp_bemf_get_output_w_phase();
																				break;
	}
	
	if(inverter_3phase_get_actual_bemf_slope()==INVERTER_BEMF_SLOPE_POSITIVE)
	{	//En pendiente positiva de BEMF
		if(aux == COMP_BEMF_OUT_SENSING_BEMF_IS_ABOVE_VREF)
			zcd_event();
	}
	else
	{
		//Sino: en pendiendte negativa de BEMF
		if(aux == COMP_BEMF_OUT_SENSING_BEMF_IS_BELOW_VREF)
			zcd_event();
	}
}


/*******************************************************************************
 * Se llama a esta funcion cada vez que estando la deteccion habilitada,
 * la tensión inducida de la rama seleccionada en"bemf_zcd_disable_detection_within_time_us"
 * atraviesa el umbral BEMF_VREF seleccionado (puede ser VBUS o NEUTRAL VIRTUAL POINT).
 *
 * Se ejecuta en contexto de interrupcion
********************************************************************************/
void bemf_comp_callback(void)
{
	zcd_event();
}

/*******************************************************************************
 * Esta funcion se encarga del "blanking" de ZCD. Deshabilitando la deteccion
 * por el tiempo pasado por parametro. Se la llama usualmente despues de ejecutar
 * un cambio de secuencia de excitacion
********************************************************************************/
void bemf_zcd_disable_detection_within_time_us (int32_t time_us)
{
	bemf_sense_zcd_sampled_end_toff_disable();
	bemf_sense_zcd_continuous_disable();
	board_tim_sctm_init_timer_with_timeout_irq_us(BOARD_TIM_SCTM_BLANKING,time_us);
}


/*******************************************************************************
 * Esta funcion se llama cuando transcurrio el tiempo de "blanking" para
 * deshabilitar la deteccion de ZCD.
 *
 * Se ejecuta en contexto de interrupcion
********************************************************************************/
void blanking_timer_expired_callback (void)
{
	if(gv.bemf_sense_type == BEMF_SENSE_TYPE_SAMPLE_END_TOFF)
	{
		bemf_sense_zcd_sampled_end_toff_enable();
	}
	else
	{ /*		bemf_sense_zcd_continous_enable(); */
		switch(inverter_3phase_get_actual_bemf_out())
		{
			case INVERTER_BEMF_ON_OUTPUT_1: board_comp_bemf_rising_enable_irq(BOARD_COMP_BEMF_U_PHASE_SELECT);
											break;
			case INVERTER_BEMF_ON_OUTPUT_2:	board_comp_bemf_rising_enable_irq(BOARD_COMP_BEMF_V_PHASE_SELECT);
											break;
			case INVERTER_BEMF_ON_OUTPUT_3:	board_comp_bemf_rising_enable_irq(BOARD_COMP_BEMF_W_PHASE_SELECT);
											break;
		}
	}
}



/*******************************************************************************
*	Solo saltara a esta funcion cuando no reciba más señal de ZCD en un tiempo
*	establecido. Ejecuta el procedimiento para frenar el motor y pone
*	el estado a FAIL
********************************************************************************/
void motor_watchdog_callback(void)
{
	motor_3phase_abort_motor(MOTOR_STOP_METHOD_FREEWHEEL,MOTOR_STATE_FAIL);
}


