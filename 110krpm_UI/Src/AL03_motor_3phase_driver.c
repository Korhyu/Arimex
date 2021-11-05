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
#define MOTOR_CHARGE_BOOTSTRAP_TIME_mS						20
/*Tiempo de alineacion del rotor a posicion conocida*/
#define MOTOR_ALIGNMENT_TIME_mS 							15
#define MOTOR_ALIGNMENT_SEQ									INVERTER_COMM_SEQ3

/*Configuracion de PWM para aplicar la alineacion del rotor - Modo Current Limit Cycle by Cycle*/
#define PWM_IN_CYCLE_BY_CYCLE_PERIOD						6000
#define PWM_IN_CYCLE_BY_CYCLE_TOFF    						300

/*Cantidad de secuencias que se va a excitar al motor sensando ZCD en modo SAMPLE AT END TOFF*/
#define STARTING_FIRST_STEPS_FROM_STAND_COUNT				12

/*Configuracion de PWM que se va a usar desde la primer conmutacion */
#define PWM_STARTING_PERIOD_uS 								100
#define PWM_STARTING_TON_uS 	  						 	30

/* Configuracion de PWM que se va a usar previo a la rampa de aceleracion */
#define PWM_OPERATING_PERIOD_uS								100
#define PWM_OPERATING_TON_uS								30
#define PWM_OPERATING_DUTY									68

/*Configuracion de PWM que se va a usar enregimen permanente */
#define PWM_OPERATING_DUTY_PER								68

#define ZCD_FRACTION_ADVANCE_START							0.33		//Porcentaje de zcd_to_zcd de avance para desde el arranque

/*Este tiempo sirve para determinar cuanto blanking aplicar al primer paso que se da*/
#define START_FIRST_STEP_DURATION_INIT_us				  	7000				//7000

/*Este blanking se usa cada vez que se pretende medir ZCD en freewheel. Depende del motor*/
#define DISABLE_ZCD_DETECTION_AFTER_FREEWHEEL_SETTING_us	200			//200

#define MAX_TIMEOUT_WATCHDOG_us 						  	30000

/*Estos parametros de aca abajo dominan la rampa de aceleracion en el arranque*/
#define SET_POINT_PWM_TON_UPDATE_TIME_mS		  		1		/*Intervalo en milisegundos en que se modifica el Ton del PWM*/
#define SET_POINT_MAX_PWM_TON_uS						95		/*Valor de TON maximo para la rampa (OJO NO PUEDE SER MENOR QUE EL PERIODO NI QUE "PWM_STARTING_TON_uS")*/
#define SET_POINT_MIN_PWM_TON_uS						10
#define SET_POINT_PWM_TON_INC_DEC_uS	 				1		/*Valor de incremento/decremento de TON */

#define SET_POINT_MAX_PWM_DUTY							95		//Valor maximo de duty aplicable al motor
#define SET_POINT_MIN_PWM_DUTY							20		//Valor minimo de duty aplicable al motor
#define SET_POINT_PWM_DUTY_INC_DEC						1		//Valor de incremento/decremento del duty
#define SET_POINT_PWM_DUTY_UPDATE_TIME_mS		  		2		//Intervalo en milisegundos en que se modifica el duty del PWM


#define TIME_TO_GET_RUNNING_TIMEOUT_mS					200

#define TIME_UPDATE_AVAIABLE							0		//Estan disponibles los datos para actualizar los tiempos de seq
#define TIME_UPDATE_READY								1		//Los tiempos de seq estan actualizados


#define MASKED_SET_POINT_PWM_TON(set_point)		(set_point-(set_point&(SET_POINT_PWM_TON_INC_DEC_uS-1)))


/*Blanking que se aplica cuando esta en close loop y correct timming*/
#define bemf_close_loop_blanking_time_calc(comm_time_us) (((comm_time_us)>>3)+((comm_time_us)>>6))

/*Calculo por default que usa para situar ZCD en la conmutacion (modificar esto modifica el avance)*/
#define __default_zcd_expected_calc(time) ((time>>2)+(time>>4))

#define DEEP_FREEWHEEL_SHIFTER							4		//Voy a dejar el freewheel por aproximadamente 64 secuencias para luego retomar el sincronismo
#define ZCD_LOST_MAX_COUNT								4		//Cantidad de ZCD LOST maxima que puedo tener antes de ir a un deep freewheel
#define ZCD_HIT_COUNT_RESET								12		//Cantidad de ZCD HIT que tengo que tener antes de restar un ZCD LOST

/*********************************************************
**********************************************************/



//******************************************
//Definiciones de uso privado en el archivo
//******************************************

//Estados de las maquina de estados del arranque del motor
#define STARTING_STATE_INIT			 					0
#define STARTING_STATE_BREAK_AND_CHARGE_BOOTSTRAP		1
#define STARTING_STATE_ALIGMENT							2
#define STARTING_STATE_FIRST_STEPS_FROM_STAND			3
#define STARTING_STATE_FREEWHEEL_PERIOD_MEASURE			4
#define STARTING_STATE_FREEWHEEL_SYNC					5
#define STARTING_STATE_STEPS_CORRECT_TIMMING			6
#define STARTING_STATE_COUNT							8

#define STARTING_SUB_STATE_UPDATING_PWM_SET_POINT		0
#define STARTING_SUB_STATE_ANALYZING_FAIL_SYNC			1
#define STARTING_SUB_STATE_CORRECTING_PARAMETERS		2
#define STARTING_SUB_STATE_RUNNING						3

#define FIRST_ALIGMENT									0
#define BREAK_ALIGMENT									1
#define SECOND_ALIGMENT									2


//Estados de BEMF en closeloop "correct timming"
#define BEMF_STATE_COMMUTATE					0
#define BEMF_STATE_BLANKING_ZCD					1
#define BEMF_STATE_WAITING_ZCD					2
#define BEMF_STATE_ZCD_DETECTED					3
#define BEMF_STATE_ZCD_LOST						4

//Tipos de sensado de ZCD
#define BEMF_SENSE_TYPE_SAMPLE_END_TOFF			0
#define BEMF_SENSE_TYPE_CONTINUOUS				1


#define STOP_RUNNING_FLAG_STOP_MOTOR			0
#define STOP_RUNNING_FLAG_DONT_STOP_MOTOR		1

#define FAIL_SYNC_RATE_SENSE_DISABLE			0
#define FAIL_SYNC_RATE_SENSE_ENABLE				1

void motor_3phase_starting_state_machine		(void);
void bemf_zcd_disable_detection_within_time_us 	(int32_t time_us);
void bemf_sense_set_type						(int32_t bemf_sense_type_p);


int32_t  motor_3phase_abort_motor				(int32_t motor_stop_method,int32_t motor_state_exit_value);


void end_of_toff_pwm_callback					(void);
void blanking_timer_expired_callback			(void);
void bemf_comp_callback							(void);
void motor_watchdog_callback					(void);
void commutation_callback						(void);

void link_zcd_expected_calculate_function		(int32_t(*func_pointer)(int32_t));

int32_t calculate_zcd_expected_default			(int32_t time_zcd_to_zcd_avg);


void exti1_callback(void);
void exti3_callback(void);
void exti5_callback(void);


//Variables globales privadas del archivo
//******************************************
int32_t (*calculate_zcd_expected)(int32_t);	//Puntero a la funcion que calcula ZCD expected

static volatile struct motor_3phase_drive gv; //Variables globales de este archivo


/*******************************************************************************
 *	JOSE - Esta funcion retorna el valor del set point del duty del pwm:
 *	
********************************************************************************/
int32_t motor_3phase_get_pwm_duty_set (void)
{
	return gv.pwm_duty_set_point;
}


/*******************************************************************************
 *	JOSE - Esta funcion escribe el valor del set point del duty del pwm:
 *	
********************************************************************************/
void motor_3phase_set_pwm_duty_set (int32_t pwm_duty_set_point)
{
	gv.pwm_duty_set_point = pwm_duty_set_point;
}

/*******************************************************************************
 *	JOSE - Esta funcion retorna el valor actual del duty del pwm:
 *	
********************************************************************************/
int32_t motor_3phase_get_pwm_duty_actual (void)
{
	return gv.pwm_duty_actual;
}



/*******************************************************************************
 *	JOSE - Esta funcion retorna el valor actual del duty del pwm:
 *	
********************************************************************************/
int32_t motor_3phase_get_pwm_duty
 (void)
{
	return gv.pwm_duty_actual;
}

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
 *	JOSE - Esta funcion setea el valor de duty del pwm que se desea aplicarse.
 *
 *	Si se elije un Ton por fuera de los limites definidos por:
 *
 *	SET_POINT_MAX_PWM_DUTY y SET_POINT_MIN_PWM_DUTY
 *
 *	Asignara saturará al limite que corresponda y retornará -1.
 *
 *	En caso que el valor de Ton deseado esté comprendido dentro de los
 *	umbrales definidos, lo asignará al set point y retornará 0
 ********************************************************************/
int32_t motor_3phase_set_pwm_duty_set_point(int32_t pwm_duty_set_point)
{
	if(pwm_duty_set_point <= SET_POINT_MAX_PWM_DUTY && pwm_duty_set_point >= SET_POINT_MIN_PWM_DUTY)
	{
		gv.pwm_duty_set_point = pwm_duty_set_point;
		return 0;
	}
	else if(pwm_duty_set_point>SET_POINT_MAX_PWM_DUTY)
	{
		gv.pwm_duty_set_point = SET_POINT_MAX_PWM_DUTY;
		return -1;
	}
	else if(pwm_duty_set_point<SET_POINT_MIN_PWM_DUTY)
	{
		gv.pwm_duty_set_point = SET_POINT_MIN_PWM_DUTY;
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
#define bemf_end_toff_sense_callback_link(func_ptr)				board_pwm_end_toff_link_callback(func_ptr)


/* JOSE - TODO: Tengo dudas sobre estas 2 y escribo las mias
 * Fran ativa o desactiva la IRQ del break en el sample del before_ton (end_of_toff) y el break no se usa en este momento
 * Asumo que esto es un error y uso la irq del CH2 que es el que esta configurado con el before_ton
 * 
*/
//TODO: Estas funciones son tentativas, despues hay que reemplazarlas por las de board_ bla bla bla y hacer las de hardware tmb
#define bemf_sense_zcd_sampled_end_toff_enable()							{TM_ClearFlag(HT_MCTM0,TM_FLAG_CH2CC); TM_IntConfig(HT_MCTM0,TM_INT_CH2CC,ENABLE);}
#define bemf_sense_zcd_sampled_end_toff_disable()							{TM_ClearFlag(HT_MCTM0,TM_FLAG_CH2CC); TM_IntConfig(HT_MCTM0,TM_INT_CH2CC,DISABLE);}
/* 
#define bemf_sense_zcd_sampled_end_toff_enable()							board_pwm_break_enable_irq()
#define bemf_sense_zcd_sampled_end_toff_disable()							board_pwm_break_disable_irq()
*/
#define bemf_sense_zcd_continous_enable()									board_comp_falling_edge_detection_only_enable(BOARD_COMP_BEMF)
#define bemf_sense_zcd_continuous_disable()									board_comp_bemf_disable_irqs_all_phases()

#define bemf_watchdog_timer_callback_link(func_ptr)							board_timer_link_callback(func_ptr,BOARD_TIM_WATCHDOG_SEL_CALLBACK)
#define bemf_watchdog_set_timeout_us(time_us)								board_tim_bftm_init_timer_with_timeout_irq_us(BOARD_TIM_BFTM_WATCHDOG,(time_us))
#define bemf_watchdog_get_count_us()										board_tim_get_bftm_count_us(BOARD_TIM_BFTM_WATCHDOG)
#define bemf_watchdog_abort()												board_tim_bftm_abort(BOARD_TIM_BFTM_WATCHDOG)

#define bemf_commutation_set_within_us(time_us)								board_tim_bftm_init_timer_with_timeout_irq_us(BOARD_TIM_BFTM_COMM,(time_us))
#define bemf_commutation_get_count_us()										board_tim_get_bftm_count_us(BOARD_TIM_BFTM_COMM)
#define bemf_commutation_abort()											board_tim_bftm_abort(BOARD_TIM_BFTM_COMM)
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

	bemf_sense_zcd_sampled_end_toff_disable();				//Antes de linkear los callbacks, me aseguro de deshabilitar las detecciones
	bemf_sense_zcd_continuous_disable();					//Porque sino hay falsas llamadas

	bemf_end_toff_sense_callback_link(end_of_toff_pwm_callback);
	
	//board_comp_bemf_link_callback(bemf_comp_callback); - Hay que linkear las 3 irqs al mismo callback.
	/*
	board_comp_bemf_rising_link_callback(bemf_comp_callback,BOARD_COMP_BEMF_U_PHASE_SELECT);
	board_comp_bemf_rising_link_callback(bemf_comp_callback,BOARD_COMP_BEMF_V_PHASE_SELECT);
	board_comp_bemf_rising_link_callback(bemf_comp_callback,BOARD_COMP_BEMF_W_PHASE_SELECT);
	*/
	board_comp_bemf_rising_link_callback(exti1_callback,BOARD_COMP_BEMF_U_PHASE_SELECT);
	board_comp_bemf_rising_link_callback(exti5_callback,BOARD_COMP_BEMF_V_PHASE_SELECT);
	board_comp_bemf_rising_link_callback(exti3_callback,BOARD_COMP_BEMF_W_PHASE_SELECT);
	

	bemf_watchdog_timer_callback_link(motor_watchdog_callback);

	bemf_commutation_callback_link(commutation_callback);	//Luego de zcd_event_freewheel_period_measure esta funcion es llamada por el timer

	gv.motor_state = MOTOR_STATE_STOPPED;

	link_zcd_expected_calculate_function(calculate_zcd_expected_default);

	//motor_3phase_set_pwm_ton_us_set_point(PWM_OPERATING_TON_uS);
	motor_3phase_set_pwm_duty_set_point(PWM_OPERATING_DUTY);

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
									motor_3phase_starting_state_machine();
									break;
		case MOTOR_STATE_RUNNING:
									//motor_3phase_starting_state_machine();
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

		//Me aseguro de tener todas las salidas desconectadas
		inverter_3phase_comm_set_seq(INVERTER_COMM_FREWHEEL, INVERTER_STATE_OVERWRITE);
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
	static int32_t timer_bootstrap, timer_alineacion, timer_to_running;
	static int32_t alignment_count;
	static int32_t zcd_count = 0;
	

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
									if (timer_alineacion == 0)
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
										
										alignment_count = FIRST_ALIGMENT;
									}

									if (board_scheduler_is_time_expired(timer_alineacion))
									{
										switch (alignment_count)
										{
											case FIRST_ALIGMENT:
												//Fin Exitacion 1 - Inicio frewwheel
												timer_alineacion = board_scheduler_load_timer(MOTOR_ALIGNMENT_TIME_mS>>1);
												inverter_3phase_comm_set_seq(INVERTER_COMM_FREWHEEL, INVERTER_STATE_OVERWRITE);
												
												alignment_count = BREAK_ALIGMENT;

												break;


											case BREAK_ALIGMENT:
												//Fin frewwheel - Inicio exitacion 2
												timer_alineacion = board_scheduler_load_timer(MOTOR_ALIGNMENT_TIME_mS);
												inverter_3phase_comm_set_seq(MOTOR_ALIGNMENT_SEQ, INVERTER_STATE_OVERWRITE);
												
												alignment_count = SECOND_ALIGMENT;

												break;


											case SECOND_ALIGMENT:
												//Fin exitacion 2
												//motor_3phase_abort_motor(INVERTER_COMM_BREAK_LOW,MOTOR_STATE_FAIL);

												gv.starting_state = STARTING_STATE_FIRST_STEPS_FROM_STAND;

												inverter_3phase_comm_next_seq();

												gv.starting_first_steps_counter=STARTING_FIRST_STEPS_FROM_STAND_COUNT;

												//Hay que setear siempre primero el periodo y despues Ton o Toff. Sino: puede fallar
												inverter_3phase_pwm_set_period_us(PWM_STARTING_PERIOD_uS);
												inverter_3phase_pwm_set_ton_us(PWM_STARTING_TON_uS);

												bemf_sense_set_type(BEMF_SENSE_TYPE_SAMPLE_END_TOFF);
																								
												gv.motor_comm_seq_period_us_avg = START_FIRST_STEP_DURATION_INIT_us;

												bemf_watchdog_set_timeout_us(START_FIRST_STEP_DURATION_INIT_us<<2);

												bemf_zcd_disable_detection_within_time_us(START_FIRST_STEP_DURATION_INIT_us>>4);

												break;
										}				
									}
									break;

		case STARTING_STATE_FIRST_STEPS_FROM_STAND:
										gv.starting_sub_state = STARTING_SUB_STATE_RUNNING;
										timer_to_running = board_scheduler_load_timer(TIME_TO_GET_RUNNING_TIMEOUT_mS);

									break;

		case STARTING_STATE_FREEWHEEL_SYNC:
		case STARTING_STATE_FREEWHEEL_PERIOD_MEASURE:
									break;

		case STARTING_STATE_STEPS_CORRECT_TIMMING:

									switch(gv.starting_sub_state)
									{
										case STARTING_SUB_STATE_UPDATING_PWM_SET_POINT:

																		gv.starting_sub_state = STARTING_SUB_STATE_RUNNING;								//Jose
																		
																		break;

										case STARTING_SUB_STATE_ANALYZING_FAIL_SYNC:

																		break;

										case STARTING_SUB_STATE_CORRECTING_PARAMETERS:

																		break;

										case STARTING_SUB_STATE_RUNNING:
																		//Regimen permanente


																		if(zcd_count == 6)
																		{
																			//Control de velocidad
																			if (gv.pwm_duty_actual != gv.pwm_duty_set_point)
																			{
																				//El set_point es diferente al duty actual por lo tanto lo modifico
																				update_pwm_duty();
																			}
																			//Actualizo el periodo del PWM
																			update_pwm_period();

																			zcd_count = 0;
																		}

																		//Sincronismo de PWM con velocidad
																		if ( gv.time_update == TIME_UPDATE_AVAIABLE )
																		{
																			//Tengo mediciones nuevas de tiempos para hacer los calculos de tiempos de conmutacion y etc.
																			calculate_times();
																			gv.time_update = TIME_UPDATE_READY;
																			zcd_count++;
																		}

																		//Recargo el timer de keep alive
																		timer_to_running = board_scheduler_load_timer(TIME_TO_GET_RUNNING_TIMEOUT_mS);

																		break;

										default: while(1);		//DEV_ERROR
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
		{
			gv.starting_first_steps_counter--;
		}

		inverter_3phase_comm_next_seq();
		//Hago un blanking de ZCD de 1/8 de lo que dura una secuencia de excitacion.
		//En esta etapa el tiempo que se mide es el de dos secuencias de exctacion (de pendiente positiva a pendiente positiva)
		//Por eso primero se divide por 2, y luego se divide por 8.
		//bemf_zcd_disable_detection_within_time_us((gv.motor_comm_seq_period_us_avg>>1)>>3);
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

		//Cambio el PWM al valor de regimen permanente
		inverter_3phase_pwm_set_period_us(PWM_OPERATING_PERIOD_uS);
		inverter_3phase_pwm_set_ton_us(PWM_OPERATING_TON_uS);
		gv.pwm_duty_actual = (inverter_3phase_pwm_get_ton_us() * 100) / inverter_3phase_pwm_get_period_us();
		gv.phase_zcd_fraction_advance = ZCD_FRACTION_ADVANCE_START;					//Adelanto que se implementa en el calculo de tiempos
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
		bemf_watchdog_set_timeout_us(MAX_TIMEOUT_WATCHDOG_us); 		//Inicio el timer para medir periodo
		//__hardware_gpio_output_set(GPIOA, 3);						//GPIO aux para monitoreo en OSC

		//inverter_3phase_comm_next_seq_pasive();
		//inverter_3phase_comm_next_seq();
		inverter_3phase_comm_next_seq_pasive();
		inverter_3phase_comm_next_seq_pasive();	 						//Voy a medir el tiempo entre 2 ZCD de pendiente POSITIVA
		bemf_zcd_disable_detection_within_time_us(gv.motor_comm_seq_period_us_avg>>3);

		inverter_3phase_comm_set_seq(INVERTER_COMM_FREWHEEL, INVERTER_STATE_NOT_OVERWRITE);
		gv.starting_state = STARTING_STATE_FREEWHEEL_PERIOD_MEASURE;
	}

	else if(gv.starting_state == STARTING_STATE_FREEWHEEL_PERIOD_MEASURE)
	{
		//Con STARTING_FIRST_STEPS_FROM_STAND_COUNT multiplo de 6 entra en este segmento en la misma seq que alineamiento
		//Como esta hoy entra en la SEQ2 significa que el callback se va a ejecutar y colocar la SEQ3

		//Estas inicializaciones de abajo son el "reset de variables" para que funcione bien CONTROL LOOP CALCULATE
		gv.time_from_zcd_to_zcd = bemf_watchdog_get_count_us(); 					//Es el tiempo entre dos ZCD de pendiente positiva				- Aprox 368us
																					//Osea es el tiempo de 2 secuencias de excitacion
				

		gv.time_from_zcd_to_zcd_avg = gv.time_from_zcd_to_zcd;						//Esta variable se usa en el CONTROL LOOP CALCULATE

		gv.motor_comm_seq_period_us = gv.time_from_zcd_to_zcd>>1;					//Correccion de la medicion para que corresponda a 1 periodo de secuencia.

		gv.motor_comm_seq_period_us_avg = gv.motor_comm_seq_period_us;

		gv.time_from_comm_to_zcd = gv.time_from_zcd_to_zcd>>2;

		gv.time_from_comm_to_zcd_avg = gv.time_from_comm_to_zcd;
		
		gv.zcd_blanking_time = (gv.time_from_zcd_to_zcd - (gv.time_from_zcd_to_zcd>>2) - (gv.time_from_zcd_to_zcd>>4));

		board_bemf_vref_select_vbus();

		bemf_commutation_set_within_us(((gv.time_from_comm_to_zcd)-11));			//Temporizo la conmutacion para hacerla dentro de medio
																					//Periodo de secuencia de excitacion. Ya que esto se ejecuta
																					//Cuando se está en ZCD. y ZCD debe caer en el medio (aprox)
																					//De la secuencia de conmutacion

																					//El "-11" es para corregir la temporizacion dado el tiempo
																					//que demora en ejecutarse instrucciones por el uC
		
		bemf_sense_zcd_continuous_disable();
		//bemf_zcd_disable_detection_within_time_us(gv.zcd_blanking_time);

		gv.starting_state = STARTING_STATE_STEPS_CORRECT_TIMMING;

		bemf_watchdog_set_timeout_us(MAX_TIMEOUT_WATCHDOG_us);

		//__hardware_gpio_output_reset(GPIOA, 3);					//GPIO aux para monitoreo en OSC
		//snap_motor_data();
	}
}

/*******************************************************************************
*	Funcion armada por Jose
*	Usando los timers calcula todos los tiempos de los diferentes eventos
*	y los carga en sus respectivas variables
*	
*	
********************************************************************************/
void calculate_times (void)
{
	//Valores medidios
	//time_from_zcd_to_zcd
	//time_from_comm_to_zcd
	//En funcion a estos fabrico los demas

	//Promedios
	common_update_average(gv.time_from_zcd_to_zcd_avg, gv.time_from_zcd_to_zcd, AVG_FACTOR_ELECTRICAL_PERIOD);
	common_update_average(gv.time_from_comm_to_zcd_avg, gv.time_from_comm_to_zcd, AVG_FACTOR_ELECTRICAL_PERIOD);
	common_update_average(gv.motor_comm_seq_period_us_avg, gv.motor_comm_seq_period_us, AVG_FACTOR_ELECTRICAL_PERIOD);

	//Blanking
	gv.zcd_blanking_time = (gv.time_from_zcd_to_zcd_avg - (gv.time_from_zcd_to_zcd_avg>>2) - (gv.time_from_zcd_to_zcd_avg>>4));
	
	gv.motor_comm_seq_period_us = (gv.time_from_zcd_to_zcd_avg>>1);

	//Error y adelanto
	//1/4 + 1/16 + 1/64 = 0.328125
	//1/4 + 1/16		= 0.3125
	gv.time_zcd_expected = (int32_t)(gv.time_from_zcd_to_zcd_avg * gv.phase_zcd_fraction_advance);
	//gv.time_zcd_expected = (gv.time_from_zcd_to_zcd_avg * 0.33);
	//gv.time_zcd_expected = (gv.time_from_zcd_to_zcd_avg>>2) + (gv.time_from_zcd_to_zcd_avg>>4) + (gv.time_from_zcd_to_zcd_avg>>6);				//Calculo por default que usa para situar ZCD en la conmutacion (modificar esto modifica el avance)
	//gv.time_zcd_expected = (gv.time_from_zcd_to_zcd_avg>>2) + (gv.time_from_zcd_to_zcd_avg>>4);
	gv.time_t_error = ((gv.time_from_zcd_to_zcd_avg>>1) - gv.motor_comm_seq_period_us);
	gv.time_t_error = gv.time_t_error + (gv.time_from_comm_to_zcd_avg - gv.time_zcd_expected);

	//Almaceno el tiempo en el que voy a hacer la proxima conmutacion
	gv.time_var_comm = gv.motor_comm_seq_period_us + gv.time_t_error;
	

	//Por alguna razon Fran actualiza estos valores LUEGO del calculo de error
	gv.motor_electrical_period_us_avg = (gv.time_from_zcd_to_zcd_avg<<1) + gv.time_from_zcd_to_zcd_avg;
	//common_update_average(gv.motor_electrical_period_us_avg, (gv.motor_comm_seq_period_us<<2) + (gv.motor_comm_seq_period_us<<1), AVG_FACTOR_ELECTRICAL_PERIOD);
}


/*******************************************************************************
*	Funcion armada por Jose
*	Esta funcion modifica el periodo del PWM para que tenga una candidad determinada
*	de periodos por conmutacion, si la velocidad es alta se usa 1 periodo por conmutacion
*	si es media son 3 y si es baja son 5.
*	De esta forma garantizo la deteccion del ZCD en bajos duty del PWM
********************************************************************************/
void update_pwm_period (void)
{
	//Actualizo el periodo para que este en sincronismo con la velocidad del motor
	//int32_t periodo_pwm = gv.motor_comm_seq_period_us_avg + (gv.motor_comm_seq_period_us_avg>>4);
	int32_t periodo_pwm;
	
	if (gv.motor_comm_seq_period_us_avg >= 100)
	{
		//Estoy en la velocidad Maxima
		//periodo_pwm = periodo_pwm / 5;		//Configuro 3 periodo del PWM por conmutacion
		periodo_pwm =  90;
	}
	else
	{
		//Estoy en la velocidad minima
		//periodo_pwm = periodo_pwm / 1;		//Configuro 1 periodos del PWM por conmutacion
		//periodo_pwm = gv.motor_comm_seq_period_us_avg;
		periodo_pwm = 90;
	}

	/*
	//Modifico 5, 3 o 1 pulso en funcion del DUTY
	if (gv.pwm_duty_actual <= MOTOR_3_PULSE_DUTY_MIN)
	{
		//Estoy en la velocidad Maxima
		//periodo_pwm = periodo_pwm / 5;		//Configuro 3 periodo del PWM por conmutacion
		periodo_pwm =  100;
	}
	else if (gv.pwm_duty_actual <= MOTOR_1_PULSE_DUTY_MIN)
	{
		//Estoy en la velocidad media
		//periodo_pwm = periodo_pwm / 3;		//Configuro 2 periodos del PWM por conmutacion
		periodo_pwm =  100;
	}
	else
	{
		//Estoy en la velocidad minima
		//periodo_pwm = periodo_pwm / 1;		//Configuro 1 periodos del PWM por conmutacion
		periodo_pwm = gv.motor_comm_seq_period_us_avg;
	}
	*/
	/*
	//Modifico 5, 3 o 1 pulso en funcion de la velocidad
	if (gv.motor_electrical_period_us_avg <= MOTOR_MAX_SPEED_PERIOD_MIN)
	{
		//Estoy en la velocidad Maxima
		periodo_pwm = periodo_pwm / 1;		//Configuro 1 periodo del PWM por conmutacion
	}
	else if (gv.motor_electrical_period_us_avg <= MOTOR_MID_SPEED_PERIOD_MIN)
	{
		//Estoy en la velocidad media
		periodo_pwm = periodo_pwm / 3;		//Configuro 3 periodos del PWM por conmutacion
	}
	else
	{
		//Estoy en la velocidad minima
		periodo_pwm = periodo_pwm / 5;		//Configuro 5 periodos del PWM por conmutacion
	}
	*/

	inverter_3phase_pwm_set_period_us(periodo_pwm);
	inverter_3phase_pwm_set_ton_us((periodo_pwm * gv.pwm_duty_actual) / 100 );
	//inverter_3phase_pwm_set_toff_us(periodo_pwm - gv.motor_comm_seq_period_us);			//PWM_OPERATING_DUTY
}

/*******************************************************************************
*	Funcion armada por Jose
*	Esta funcion modifica el duty del PWM utilizando una rampa
*	Cuando el UI modifica el gv.pwm_duty_set_point esta funcion lleva el duty actual
*	a ese set point en forma gradual
********************************************************************************/
void update_pwm_duty (void)
{
	static int32_t timer_ramp = 0;
	int32_t periodo = gv.motor_comm_seq_period_us;

	
	if(board_scheduler_is_time_expired(timer_ramp))
	{
		timer_ramp = board_scheduler_load_timer(SET_POINT_PWM_DUTY_UPDATE_TIME_mS);

		if( (gv.pwm_duty_actual + SET_POINT_PWM_DUTY_INC_DEC) <= gv.pwm_duty_set_point )
		{
			gv.pwm_duty_actual += SET_POINT_PWM_DUTY_INC_DEC;
		}
		else if ((gv.pwm_duty_actual - SET_POINT_PWM_DUTY_INC_DEC) >= gv.pwm_duty_set_point)
		{
			gv.pwm_duty_actual -= SET_POINT_PWM_DUTY_INC_DEC;
		}
	}
	else if (timer_ramp==0)
	{
		timer_ramp = board_scheduler_load_timer(SET_POINT_PWM_DUTY_UPDATE_TIME_mS);
	}
}



/*******************************************************************************
*	Funcion re armada por Jose
*	La idea de esta funcion es que tome los tiempos al momento de detectar
*	un ZCD, con estos tiempos se hacen las proximas conmutaciones.
*	Luego desactiva la deteccion de ZCD hasta el proximo con pendiente positiva 
*	(aprox) y asi sigue
********************************************************************************/
void zcd_event_correct_timing_comm(void)
{
	//Tomo el tiempo entre zcd positivos 
	gv.time_from_zcd_to_zcd = bemf_watchdog_get_count_us();

	//Tomo el tiempo entre la ultima conmutacion (pendiente -) y el zcd+
	gv.time_from_comm_to_zcd = bemf_commutation_get_count_us();

	//Configuro el watchdog de proteccion y lo reseteo para la medicion de los proximos zcd +
	bemf_watchdog_set_timeout_us(gv.time_from_zcd_to_zcd<<3);		//Si da un poco mas de 1 vuelta sin zcd detengo el motor

	bemf_sense_zcd_continuous_disable();

	gv.bemf_state = BEMF_STATE_ZCD_DETECTED;
	gv.time_update = TIME_UPDATE_AVAIABLE;
}


/*******************************************************************************
*	Funcion re armada por Jose
*	
*
*	
 *******************************************************************************/
void commutation_callback(void)
{
	static int16_t zcd_lost_count = 0;						//Contador de ZCD LOST para calcular fail ratio
	static int16_t zcd_hit_count = 0;						//ZCD exitosos luego del primer LOST para calcular fail ratio

	if(gv.stop_running_flag != STOP_RUNNING_FLAG_STOP_MOTOR)
	{
		if(inverter_3phase_get_actual_bemf_slope()==INVERTER_BEMF_SLOPE_POSITIVE)
		{
			if(gv.bemf_state != BEMF_STATE_ZCD_DETECTED)
			{
				//ZCD LOST!!
				zcd_lost_count++;			//Conteo de ZCD lost para mandar a un "deep freewheel"
				
				//inverter_3phase_comm_next_seq_pasive();

				gv.bemf_state = BEMF_STATE_ZCD_LOST;
				inverter_3phase_comm_set_seq(INVERTER_COMM_FREWHEEL, INVERTER_STATE_NOT_OVERWRITE);
					
				if ( zcd_lost_count > ZCD_LOST_MAX_COUNT )
				{
					/* -------------------------------------- DEEP FREEWHEEL -------------------------------------- */
					//Tengo un alto indice de ZCD LOST y tengo que pasar a "deep freewheel"
					zcd_lost_count = 0;		//Reseteo el contador
					bemf_zcd_disable_detection_within_time_us(gv.motor_comm_seq_period_us_avg<<DEEP_FREEWHEEL_SHIFTER);
				}
				else
				{
					//Si se pierde la deteccion, se vuelve a FREEWHEEL a detectar ZCD y volver a medir el periodo electrico de una secuencia.
					bemf_zcd_disable_detection_within_time_us(gv.motor_comm_seq_period_us_avg>>1);
				}	
				board_bemf_vref_select_neutral_point();
				gv.starting_state = STARTING_STATE_FREEWHEEL_SYNC;
			}
			else
			{
				//Entre por una secuencia que tiene un zcd+ lo primero que hago es hacer el cambio de seq
				inverter_3phase_comm_next_seq();

				//Ahora estoy en una seq con zcd-
				bemf_commutation_set_within_us(gv.time_var_comm - 10);

				//Habilito la deteccion de zcd luego del pulso del cambio de secuencia.
				bemf_zcd_disable_detection_within_time_us(gv.time_var_comm + 10);
				
				gv.bemf_state = BEMF_STATE_COMMUTATE;


				/* ---------------------------------- DEEP FREEWHEEL COUNTER --------------------------------- */
				if ( zcd_lost_count > 0 )
				{
					//Si sucedio algun ZCD LOST, comienzo el control, cada ZCD_HIT_COUNT_RESET de ZCD exitosos, resto un ZCD LOST
					//Si el contador de ZCD LOST supera cierto valor, mando a deep freewheel
					zcd_hit_count++;

					if( zcd_hit_count >= ZCD_HIT_COUNT_RESET)
					{
						zcd_lost_count--;		//Resto un ZCD LOST
						zcd_hit_count = 0;		//Reinicio la cuenta para la proxima resta
					}
				}
			}
		}
		else
		{
			//Entre por una secuencia que tiene un zcd- lo primero que hago es hacer el cambio de seq
			inverter_3phase_comm_next_seq();
			
			//Ahora estoy en una seq con zcd+
			bemf_commutation_set_within_us((gv.motor_comm_seq_period_us));

			gv.bemf_state = BEMF_STATE_WAITING_ZCD;	
		}
	}
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
		if(gv.starting_state==STARTING_STATE_STEPS_CORRECT_TIMMING)
		{
			zcd_event_correct_timing_comm();
		}
		else if(gv.starting_state==STARTING_STATE_FREEWHEEL_PERIOD_MEASURE || gv.starting_state==STARTING_STATE_FREEWHEEL_SYNC)
		{
			zcd_event_freewheel_period_measure();
		}
		else if(gv.starting_state == STARTING_STATE_FIRST_STEPS_FROM_STAND)
		{
			zcd_event_first_steps_state();
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


void exti1_callback(void)
{
	//ZCD en fase U
	zcd_event();
}

void exti3_callback(void)
{
	//ZCD en fase W
	zcd_event();
}

void exti5_callback(void)
{
	//ZCD en fase V
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
	board_tim_sctm_init_timer_with_timeout_irq_us(BOARD_TIM_SCTM_BLANKING, time_us);

	//Cuando vence este timer se llama a blanking_timer_expired_callback 
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
		//Deteccion en BEMF_SENSE_TYPE_SAMPLE_END_TOFF
		bemf_sense_zcd_sampled_end_toff_enable();
	}
	else
	{
		//Deteccion en BEMF_SENSE_TYPE_CONTINUOUS
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
	//Garantizar arranque
	if (gv.starting_state != STARTING_STATE_STEPS_CORRECT_TIMMING)
	{
		static int8_t starting_fail_count = 0;			//Contador de intentos fallidos de arranque
		starting_fail_count++;

		//Si no estoy en CORRECT_TIMMING es que estoy en arranque
		if ( starting_fail_count < FAIL_STARTS_MAX_COUNT)
		{
			//Si fallo durante los primeros pasos vuelvo a etapas anteriores para intentar de nuevo
			if(gv.starting_state == STARTING_STATE_FIRST_STEPS_FROM_STAND)
			{
				inverter_3phase_comm_set_seq(INVERTER_COMM_FREWHEEL, INVERTER_STATE_OVERWRITE);
				gv.starting_state = STARTING_STATE_INIT;	//Vuelvo al principio para darle tiempo a un frenado, carga de bootsrap y reinicio
			}
			else
			{
				//Fallo en otra etapa. Hay que ver donde puede fallar y que accion tomar
			}
		}
		else
		{
			motor_3phase_abort_motor(MOTOR_STOP_METHOD_FREEWHEEL,MOTOR_STATE_FAIL);
		}
	}
	else
	{
		//Si estoy en CORRECT_TIMMING es que estoy en funcionamiento normal y algo paso
		motor_3phase_abort_motor(MOTOR_STOP_METHOD_FREEWHEEL,MOTOR_STATE_FAIL);
	}
}





/*******************************************************************************
*	Funciones de modificacion de Velocidad de Jose
*	
*	Por como se plantea el funcionamiento hoy (lazo abierto) se modifica el PWM
*	Entre 3 posibles valores para obtener las diferentes velocidades
 *******************************************************************************/
void motor_3phase_speed_change (int32_t modif)
{
	//Paso al siguiente set del PWM
	int32_t actual_pwm_set = 0;

	actual_pwm_set = motor_3phase_get_pwm_ton_us_set_point();

	if(modif == MORE_SPEED)
		if(actual_pwm_set < MOTOR_MAX_SPEED_PWM)
		{
			//No estoy en la velocidad maxima
			if(actual_pwm_set < MOTOR_MID_SPEED_PWM)
			{
				//Estoy en la velocidad minima
				gv.pwm_duty_set_point = MOTOR_MID_SPEED_PWM;
			}
			else
			{
				//Estoy en la velocidad media
				gv.pwm_duty_set_point = MOTOR_MAX_SPEED_PWM;
			}
		}
		else
		{
			//Ya estoy en la velocidad maxima
		}
	else
	{
		if(actual_pwm_set > MOTOR_MIN_SPEED_PWM)
		{
			//No estoy en la velocidad maxima
			if(actual_pwm_set > MOTOR_MID_SPEED_PWM)
			{
				//Estoy en la velocidad maxima
				gv.pwm_duty_set_point = MOTOR_MID_SPEED_PWM;
			}
			else
			{
				//Estoy en la velocidad media
				gv.pwm_duty_set_point = MOTOR_MIN_SPEED_PWM;
			}
		}
		else
		{
			//Ya estoy en la velocidad minima
		}
	}
}


/*******************************************************************************
*	Funciones de modificacion de adelanto de Jose
*	
*	Esta funcion modifica el valor de la variable gv.phase_zcd_fraction_advance
*	para poder modificar en forma dinamica el avance utilizado en el motor
 *******************************************************************************/
void motor_3phase_phase_lead_change (int32_t modif)
{
	#define	TABLE_MAX	11

	static int8_t index = 5;
	const float lead_table[TABLE_MAX] = {0.25, 0.27, 0.29, 0.31, 0.32, 0.33, 0.34, 0.35, 0.37, 0.39, 0.41};
	
	if (modif == MORE_LEAD)
	{
		index++;
		if (index >= TABLE_MAX)
		{
			index = TABLE_MAX;
		}
	}
	else
	{
		index--;
		if (index <= 0)
		{
			index = 0;
		}
	}
	
	gv.phase_zcd_fraction_advance = lead_table[index];
}




/*******************************************************************************
 * Funcion para enviar datos del motor
 * 
 * 
 *******************************************************************************/
static int32_t data_send = FALSE;
void send_motor_data (struct motor_3phase_drive *p)
{
	printf("\n*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-");
	printf("time_from_zcd_to_zcd = %d\n",p->time_from_zcd_to_zcd);
	printf("time_from_zcd_to_zcd_avg = %d\n",p->time_from_zcd_to_zcd_avg);

	printf("motor_comm_seq_period_us = %d\n",p->motor_comm_seq_period_us);
	printf("motor_comm_seq_period_us_avg = %d\n",p->motor_comm_seq_period_us_avg);

	printf("time_from_comm_to_zcd = %d\n",p->time_from_comm_to_zcd);
	printf("time_from_comm_to_zcd_avg = %d\n",p->time_from_comm_to_zcd_avg);
	
	printf("motor_electrical_period_us_avg = %d\n",p->motor_electrical_period_us_avg);
	printf("zcd_blanking_time = %d\n",p->zcd_blanking_time);
	printf("time_zcd_expected = %d\n",p->time_zcd_expected);
	printf("time_t_error = %d\n",p->time_t_error);
	printf("time_var_comm = %d\n",p->time_var_comm);
	printf("*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n");
}

#define MAX_DATA 48
static int i=0;
void snap_motor_data (void)
{
	static struct motor_3phase_drive ms[MAX_DATA];		//Motor Snap data

	ms[i].motor_comm_seq_period_us = gv.motor_comm_seq_period_us;
	ms[i].time_from_comm_to_zcd = gv.time_from_comm_to_zcd;
	ms[i].time_from_zcd_to_zcd = gv.time_from_zcd_to_zcd;
	ms[i].time_zcd_expected = gv.time_zcd_expected;
	ms[i].time_t_error = gv.time_t_error;
	ms[i].motor_comm_seq_period_us_avg = gv.motor_comm_seq_period_us_avg;
	ms[i].time_from_comm_to_zcd_avg = gv.time_from_comm_to_zcd_avg;
	ms[i].time_from_zcd_to_zcd_avg = gv.time_from_zcd_to_zcd_avg;
	ms[i].motor_electrical_period_us_avg = gv.motor_electrical_period_us_avg;
	ms[i].time_var_comm = gv.time_var_comm;

	ms[i].zcd_blanking_time = gv.zcd_blanking_time;

	i++;
	if (i > MAX_DATA)
	{
		inverter_3phase_comm_set_seq(INVERTER_COMM_FREWHEEL, INVERTER_STATE_NOT_OVERWRITE);
		gv.stop_running_flag = STOP_RUNNING_FLAG_STOP_MOTOR;

		for(i=0; i < MAX_DATA; i++)
		{
			send_motor_data(&ms[i]);
		}
	}
}
