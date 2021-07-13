#include "motor_trifase.h"
#include "scheduler.h"
#include "bemf_machine_state.h"


void motor_init (volatile struct motor_characteristic * motor_ptr)
{	
	motor_ptr->state = MOTOR_STATE_STOPPED;
	motor_ptr->motor_spin_direction = MOTOR_SPIN_DIRECTION_FOWARD;
	motor_ptr->motor_current_comm_seq = MOTOR_COMM_FREEWHEEL;
}
	


void motor_stop_procedure(void)
{
	__hardware_tim2_abort();
	__hardware_tim3_abort();
	zcd_sense_disable(&motor);
	motor_driver_set_to(&motor,MOTOR_COMM_FREEWHEEL,MOTOR_COMM_STATE_OVERWRITE);
	motor.state = MOTOR_STATE_STOPPED;
}


#define __SYSTEM_STATUS_ERROR_OVERTEMP_FOR_MOTOR_RUNNING 					(1<<0)
#define __SYSTEM_STATUS_ERROR_OVERTEMP_FOR_MOTOR_START_UP					(1<<1)
#define __SYSTEM_STATUS_ERROR_SUB_VOLTAGE_VBUS_FOR_MOTOR_RUNNING	(1<<2)
#define __SYSTEM_STATUS_ERROR_SUB_VOLTAGE_VBUS_FOR_MOTOR_START_UP (1<<3)

#define __SYSTEM_STATUS_POWER_STAGE_MAX_TEMP_FOR_STARTUP 45
#define __SYSTEM_STATUS_POWER_STAGE_MAX_TEMP_FOR_RUNNING 50
#define __SYSTEM_STATUS_VBUS_MINIMUN_VOLTAGE_FOR_STARTUP 30
#define __SYSTEM_STATUS_VBUS_MINIMUN_VOLTAGE_FOR_RUNNING 30

int32_t system_status_conditions_are_not_ok_for_motor_op (void)
{
	int32_t return_flags;
	return_flags=0;
	if(motor.state == MOTOR_STATE_STOPPED)
	{
			if(motor.power_stage_temp_C > __SYSTEM_STATUS_POWER_STAGE_MAX_TEMP_FOR_STARTUP)
				return_flags+=__SYSTEM_STATUS_ERROR_OVERTEMP_FOR_MOTOR_START_UP;
			if(motor.vbus_voltage_V < __SYSTEM_STATUS_VBUS_MINIMUN_VOLTAGE_FOR_STARTUP)
				return_flags+=__SYSTEM_STATUS_ERROR_SUB_VOLTAGE_VBUS_FOR_MOTOR_START_UP;
	}
	else
	{
			if(motor.power_stage_temp_C > __SYSTEM_STATUS_POWER_STAGE_MAX_TEMP_FOR_RUNNING)
				return_flags+=__SYSTEM_STATUS_ERROR_OVERTEMP_FOR_MOTOR_START_UP;
			if(motor.vbus_voltage_V < __SYSTEM_STATUS_VBUS_MINIMUN_VOLTAGE_FOR_RUNNING)
				return_flags+=__SYSTEM_STATUS_ERROR_SUB_VOLTAGE_VBUS_FOR_MOTOR_START_UP;
	}
	return return_flags;
}



#define __MOTOR_RE_STARTUP_MAX_COUNT_FAILS_TO_RESTART 3
#define __MOTOR_RE_STARTUP_DELAY_AFTER_FAIL 10
void motor_start_procedure(void)
{
  if(motor.state == MOTOR_STATE_STOPPED && (system_status_conditions_are_not_ok_for_motor_op()==0))
	{
		motor.state = MOTOR_STATE_STARTING;
	}
	
	if(system_status_conditions_are_not_ok_for_motor_op())
	{
		motor_stop_procedure();
	}
}






void motor_task(volatile struct motor_characteristic * motor_ptr)
{
	static int32_t tiempo_bootstrap,tiempo_alineacion;
	
	motor_start_procedure();
	
	switch(motor_ptr->state)
	{
		case MOTOR_STATE_STOPPED:
															motor_ptr->starting_state = MOTOR_STARTING_INIT;
															break;
		
				/*-------------------------------------------------------*/
				/*-------Inicio Etapas para el arranque del motor--------*/
				/*-------------------------------------------------------*/
		case MOTOR_STATE_STARTING:
			
				switch(motor_ptr->starting_state)
				{
						case MOTOR_STARTING_INIT:	
																			motor_ptr->starting_state = MOTOR_STARTING_INIT;
																			tiempo_bootstrap = 0;
																			tiempo_alineacion =0;
																			printf("Passing from STOPPED to STARTING\n\r");
							
																			motor_ptr->starting_state = MOTOR_STARTING_CHARGE_BOOTSTRAP;
																			break;
						
						
						case MOTOR_STARTING_CHARGE_BOOTSTRAP: //Cargo capacitores de bootstraps				

																			if(tiempo_bootstrap==0)	//Esta variable se inicializa en 0 si entra a "MOTOR_STATE_STOPPED"
																			{
																				tiempo_bootstrap = scheduler_next_time_exec(MOTOR_CHARGE_BOOTSTRAP_TIME_mS);
																				
																				motor_driver_set_to(motor_ptr,MOTOR_COMM_BREAK_LOW,MOTOR_COMM_STATE_OVERWRITE);
																			}	
																			else if(scheduler_is_time_to_exec(tiempo_bootstrap))
																			{
																				motor_ptr->starting_state = MOTOR_STARTING_ALIGNMENT;							
																			}
																			break;
																			
						case MOTOR_STARTING_ALIGNMENT: 			//Primer secuencia de excitacion: Para alinear a posicion conocida

																			if(tiempo_alineacion==0)//Esta variable se inicializa en 0 si entra a "MOTOR_STATE_STOPPED"
																			{
																				  //Prendo comparador de corriente y aseguro que el de bemf este apagado 
																				__hardware_comp_i_peak_enable_sense();
																				__hardware_comp_bemf_disable_irq();
																				__hardware_comp_bemf_disable_sense();
																					//Seteo atenuador desactivado (para sensar en Toff)
																				__hardware_bemf_attenuation_disable();
																					//Comparo BEMFs contra VUBS/2
																				__hardware_bemf_vref_select_vbus();
																					//Solo sensa BEMF en TOFF
																				__hardware_bemf_blanking_mode_set_blanking_on_ton_only();
																					//Seteo parametros para excitacion de "PWM" por corriente pico
																					hardware_pwm_set_period_us(3000);
																					hardware_pwm_set_toff_us(300);																			
																																								
																					tiempo_alineacion = scheduler_next_time_exec(MOTOR_ALGIMENT_TIME_mS);
																					motor_driver_set_to(motor_ptr,MOTOR_COMM_SEQ4,MOTOR_COMM_STATE_OVERWRITE);
																			}
																			else if (scheduler_is_time_to_exec(tiempo_alineacion))
																			{
																					hardware_pwm_set_time_for_sense_zcd_before_ton_starts_us(10);	//10uS antes de empezar TON va a sensar ZCD
																				
																					motor_ptr->discharge_inductance_time_us = 0;
																				
																					motor_ptr->starting_state = MOTOR_STARTING_FIRST_STEPS_FROM_STAND;
																																										
																					motor_driver_next_seq(motor_ptr);								//Hago un paso de 120° como sugieren muchas AppNotes
																					motor_driver_next_seq(motor_ptr);																			
																				
																					motor_ptr->comm_time_avg_us = 8000;							//La rutina de ZCD tiene una parte para descartar falsos ZCD,
																																													//Inicializando esto asi, estoy obligando a que descarte detecciones
																																													//Que sucedan antes de ese tiempo.
																		
																				__hardware_comp_bemf_enable_sense();							//Ojo que habilitar el comparador hace saltar la IRQ 						

																					motor_ptr->starting_first_steps_count = 30;//20; //1000; //poner en 5000 para el motor chino de 100milrpm		
																					hardware_pwm_set_period_us(100);
																					hardware_pwm_set_ton_us(30);
																				  zcd_sense_set_type(motor_ptr,ZCD_SENSE_TYPE_PUNTUAL_SAMPLE_END_TOFF_ONLY); // Sensar Solo Toff
																					set_blanking_configure(&motor,4000);	
																			}
																			break;
						
																			
						case MOTOR_STARTING_FIRST_STEPS_FROM_STAND:
																			break;
						
						case MOTOR_STARTING_STEPS_IN_CORRECT_TIMMING:
																			break;
									
				}
				break;//Este break es del case "MOTOR_STATE_STARTING" 
						/*-------------------------------------------------------*/
						/*--------Fin de etapas para el arranque del motor-------*/
						/*-------------------------------------------------------*/
				
		case MOTOR_STATE_FAIL:
							break;
				
		default:	
							motor_driver_set_to(motor_ptr,MOTOR_COMM_FREEWHEEL,MOTOR_COMM_STATE_OVERWRITE);
							__hardware_led_overtemp_set();
							__hardware_led_fail_sync_reset();
							while(1)
							{
								__hardware_led_overtemp_toggle();
								__hardware_led_fail_sync_toggle();								
								HAL_Delay(300);
								printf("Error! - motor.state UNKNOWN\n\r");
							}
							break;
	}
}



void hardware_comp_i_peak_callback(void)
{
		hardware_pwm_set_outputs_to_toff();		//Si es que esta funcionando con control de corriente pico en vez de PWM clasico
}









//Activa / Desactiva las salidas de PWM - Señales de sensado segun en que secuencia se encuentre
/////////////////////////////////////////////////////////////////////////////
// TRAPEZOIDAL COMMUTATION SIGNALS AND PHASES
//
//    4|5|6|1|2|3|4|5|6|1|2|3|
//     | | |_|_| | | | |_|_| |
//  U _|_|/| | |\|_|_|/| | |\|
//    _| | | | |_|_| | | | |_|
//  V  |\|_|_|/| | |\|_|_|/| |
//     |_|_| | | | |_|_| | | |
//  W /| | |\|_|_|/| | |\|_|_|
//
// 	SEQ   Low   High  Comparator
//   1     V     U       -W (negative slope)
//   2     W     U        V (positive slope)
//   3     W     V       -U 
//   4     U     V        W
//   5     U     W       -V
//   6     V     W        U
//
/////////////////////////////////////////////////////////////////////////////

int32_t motor_driver_set_to (volatile struct motor_characteristic * motor_ptr, uint32_t motor_sequence, uint32_t motor_comm_state)
{
	uint32_t aux;
	
	if(motor_comm_state == MOTOR_COMM_STATE_PRESERVE)
	{	
		aux = motor_ptr->motor_current_comm_seq;
	}

	switch(motor_sequence)
	{
		case MOTOR_COMM_SEQ1:
					
								motor_ptr->bemf_slope=MOTOR_BEMF_SLOPE_NEGATIVE;
			
								motor_ptr->comp_bemf_detection_edge_selection 	= COMP_BEMF_ZCD_COMP_OUT_SEQ1_DETECTION_EDGE;
								motor_ptr->comp_bemf_level_lost_zcd_detect			= COMP_BEMF_ZCD_COMP_OUT_SEQ1_ZCD_LOST_DETECT;
								motor_ptr->comp_bemf_level_zcd_not_detected_yet = COMP_BEMF_ZCD_COMP_OUT_SEQ1_ZCD_NOT_DETECTED_YET;
		
								motor_ptr->motor_current_comm_seq = MOTOR_COMM_SEQ1;
		
							
		
							__hardware_pwm_hin3_reset();			//W-: COMP
							__hardware_pwm_lin3_reset();
		
							__hardware_pwm_lin1_reset();			//U: High
							__hardware_pwm_hin1_set();
		
							__hardware_pwm_hin2_reset();			//V: Low
							__hardware_pwm_lin2_set();
			
							__hardware_bemf_slope_to_sense_negative();
							__hardware_comp_bemf_sense_w_phase();	
							break;
		
		case MOTOR_COMM_SEQ2:
			
								motor_ptr->bemf_slope=MOTOR_BEMF_SLOPE_POSITIVE;
			
								motor_ptr->comp_bemf_detection_edge_selection 	= COMP_BEMF_ZCD_COMP_OUT_SEQ2_DETECTION_EDGE;
								motor_ptr->comp_bemf_level_lost_zcd_detect			= COMP_BEMF_ZCD_COMP_OUT_SEQ2_ZCD_LOST_DETECT;
								motor_ptr->comp_bemf_level_zcd_not_detected_yet = COMP_BEMF_ZCD_COMP_OUT_SEQ2_ZCD_NOT_DETECTED_YET;
		
								motor_ptr->motor_current_comm_seq = MOTOR_COMM_SEQ2;
					
							__hardware_pwm_hin2_reset();			//V+: COMP
							__hardware_pwm_lin2_reset();
							
							__hardware_pwm_lin1_reset();			//U: High
							__hardware_pwm_hin1_set();			
		
							__hardware_pwm_hin3_reset();			//W: Low
							__hardware_pwm_lin3_set();		
		
							__hardware_bemf_slope_to_sense_positive();
							__hardware_comp_bemf_sense_v_phase();	
							break;
		
		case MOTOR_COMM_SEQ3:	

								motor_ptr->bemf_slope=MOTOR_BEMF_SLOPE_NEGATIVE;
			
								motor_ptr->comp_bemf_detection_edge_selection 	= COMP_BEMF_ZCD_COMP_OUT_SEQ3_DETECTION_EDGE;
								motor_ptr->comp_bemf_level_lost_zcd_detect			= COMP_BEMF_ZCD_COMP_OUT_SEQ3_ZCD_LOST_DETECT;
								motor_ptr->comp_bemf_level_zcd_not_detected_yet = COMP_BEMF_ZCD_COMP_OUT_SEQ3_ZCD_NOT_DETECTED_YET;
		
								motor_ptr->motor_current_comm_seq = MOTOR_COMM_SEQ3;
		
							__hardware_pwm_hin1_reset();			//U-: COMP
							__hardware_pwm_lin1_reset(); 
							
							__hardware_pwm_lin2_reset();			//V: High
							__hardware_pwm_hin2_set();			
		
							__hardware_pwm_hin3_reset();
							__hardware_pwm_lin3_set();				//W: Low
		
							__hardware_bemf_slope_to_sense_negative();
							__hardware_comp_bemf_sense_u_phase();	
							break;

		case MOTOR_COMM_SEQ4:

								motor_ptr->bemf_slope=MOTOR_BEMF_SLOPE_POSITIVE;
			
								motor_ptr->comp_bemf_detection_edge_selection 	= COMP_BEMF_ZCD_COMP_OUT_SEQ4_DETECTION_EDGE;
								motor_ptr->comp_bemf_level_lost_zcd_detect			= COMP_BEMF_ZCD_COMP_OUT_SEQ4_ZCD_LOST_DETECT;
								motor_ptr->comp_bemf_level_zcd_not_detected_yet = COMP_BEMF_ZCD_COMP_OUT_SEQ4_ZCD_NOT_DETECTED_YET;
		
								motor_ptr->motor_current_comm_seq = MOTOR_COMM_SEQ4;
		
							__hardware_pwm_hin3_reset();			//W: COMP
							__hardware_pwm_lin3_reset();
							
							__hardware_pwm_lin2_reset();			//V: High
							__hardware_pwm_hin2_set();			
		
							__hardware_pwm_hin1_reset();			//U: Low
							__hardware_pwm_lin1_set();	
		
							__hardware_bemf_slope_to_sense_positive();
							__hardware_comp_bemf_sense_w_phase();	
							break;
		
		case MOTOR_COMM_SEQ5:
	
								motor_ptr->bemf_slope=MOTOR_BEMF_SLOPE_NEGATIVE;
			
								motor_ptr->comp_bemf_detection_edge_selection 	= COMP_BEMF_ZCD_COMP_OUT_SEQ5_DETECTION_EDGE;
								motor_ptr->comp_bemf_level_lost_zcd_detect			= COMP_BEMF_ZCD_COMP_OUT_SEQ5_ZCD_LOST_DETECT;
								motor_ptr->comp_bemf_level_zcd_not_detected_yet = COMP_BEMF_ZCD_COMP_OUT_SEQ5_ZCD_NOT_DETECTED_YET;
		
								motor_ptr->motor_current_comm_seq = MOTOR_COMM_SEQ5;
		
							__hardware_pwm_hin2_reset();
							__hardware_pwm_lin2_reset();
							
							__hardware_pwm_lin3_reset();
							__hardware_pwm_hin3_set();			
							
							__hardware_pwm_hin1_reset();
							__hardware_pwm_lin1_set();	
		
							__hardware_bemf_slope_to_sense_negative();
							__hardware_comp_bemf_sense_v_phase();							
							break;
		
		case MOTOR_COMM_SEQ6:	
			
								motor_ptr->bemf_slope=MOTOR_BEMF_SLOPE_POSITIVE;
			
								motor_ptr->comp_bemf_detection_edge_selection 	= COMP_BEMF_ZCD_COMP_OUT_SEQ6_DETECTION_EDGE;
								motor_ptr->comp_bemf_level_lost_zcd_detect			= COMP_BEMF_ZCD_COMP_OUT_SEQ6_ZCD_LOST_DETECT;
								motor_ptr->comp_bemf_level_zcd_not_detected_yet = COMP_BEMF_ZCD_COMP_OUT_SEQ6_ZCD_NOT_DETECTED_YET;
		
								motor_ptr->motor_current_comm_seq = MOTOR_COMM_SEQ6;
		
							__hardware_pwm_hin1_reset();
							__hardware_pwm_lin1_reset();
							
							__hardware_pwm_lin3_reset();
							__hardware_pwm_hin3_set();			
		
							__hardware_pwm_hin2_reset();
							__hardware_pwm_lin2_set();	
		
							__hardware_bemf_slope_to_sense_positive();
							__hardware_comp_bemf_sense_u_phase();	
							break;		
		
		case MOTOR_COMM_FREEWHEEL:
							__hardware_pwm_lin1_reset();
							__hardware_pwm_lin2_reset();
							__hardware_pwm_lin3_reset();
							__hardware_pwm_hin1_reset();
							__hardware_pwm_hin2_reset();
							__hardware_pwm_hin3_reset();
		
							motor_ptr->motor_current_comm_seq = MOTOR_COMM_FREEWHEEL;
		
							break;
		
		case MOTOR_COMM_BREAK_LOW:
							__hardware_pwm_lin1_set();
							__hardware_pwm_lin2_set();
							__hardware_pwm_lin3_set();
							__hardware_pwm_hin1_reset();
							__hardware_pwm_hin2_reset();
							__hardware_pwm_hin3_reset();
		
							motor_ptr->motor_current_comm_seq = MOTOR_COMM_BREAK_LOW;
		
							break;

		case MOTOR_COMM_BREAK_HIGH:
							__hardware_pwm_lin1_reset();
							__hardware_pwm_lin2_reset();
							__hardware_pwm_lin3_reset();
							__hardware_pwm_hin1_set();
							__hardware_pwm_hin2_set();
							__hardware_pwm_hin3_set();		
		
							motor_ptr->motor_current_comm_seq = MOTOR_COMM_BREAK_HIGH;
		
							break;
		
		default:  
							return -1;
	}	
	
	if(motor_comm_state == MOTOR_COMM_STATE_PRESERVE)
	{		
		motor_ptr->motor_current_comm_seq = aux;
	}
	hardware_pwm_start_from_0();
	
	return 0;	
	
}

//Ejecuta conmuta a la proxima secuencia de excitacion
int32_t motor_driver_next_seq (volatile struct motor_characteristic * motor_ptr)
{
	
	if(motor_ptr->motor_current_comm_seq < MOTOR_COMM_SEQ_COUNT)
	{
		if(motor_ptr->motor_spin_direction == MOTOR_SPIN_DIRECTION_REVERSE)
		{
			if(motor_ptr->motor_current_comm_seq == MOTOR_COMM_SEQ1)
				motor_ptr->motor_current_comm_seq = MOTOR_COMM_SEQ6;
			else
				motor_ptr->motor_current_comm_seq -- ;
		}
		else
		{
			if(motor_ptr->motor_current_comm_seq == MOTOR_COMM_SEQ6)
				motor_ptr->motor_current_comm_seq = MOTOR_COMM_SEQ1;
			else
				motor_ptr->motor_current_comm_seq ++ ;
		}
	}
	
	else if(motor_ptr->motor_current_comm_seq == MOTOR_COMM_BREAK_LOW ||
					motor_ptr->motor_current_comm_seq == MOTOR_COMM_BREAK_HIGH)
	{
				motor_ptr->motor_current_comm_seq = MOTOR_COMM_FREEWHEEL;	
	}
	else if (motor_ptr->motor_current_comm_seq == MOTOR_COMM_FREEWHEEL)
	{
				motor_ptr->motor_current_comm_seq = MOTOR_COMM_SEQ1;
	}
	else
	{
				motor_ptr->motor_current_comm_seq = MOTOR_COMM_FREEWHEEL;
				return -1;
	}
	
	motor_driver_set_to(motor_ptr,motor_ptr->motor_current_comm_seq,MOTOR_COMM_STATE_OVERWRITE);
	return 0;
}








//Configuro Atenuador, modo de Blanking y tipo de IRQ de sampleo segun el modo que se pasa
int32_t zcd_sense_set_type(volatile struct motor_characteristic * motor_ptr,uint32_t modo)
{
	if(modo<ZCD_SENSE_TYPE_COUNT)
		motor_ptr->bemf_sense_type = modo;
	else
		return -1;
	
	if		(motor_ptr->bemf_sense_type& (ZCD_SENSE_TYPE_CONTINOUS_ON_TOFF_ONLY|
																			ZCD_SENSE_TYPE_PUNTUAL_SAMPLE_END_TOFF_ONLY))
	{	//Sensados en TOFF
		__hardware_bemf_attenuation_disable();
		//__hardware_bemf_blanking_mode_set_blanking_on_ton_only();
		__hardware_bemf_blanking_mode_set_blanking_disable();
		__hardware_pwm_set_irq_at_end_toff_only();
	}
	else if(motor_ptr->bemf_sense_type == ZCD_SENSE_TYPE_CONTINUOUS_WITHOUT_BLANK_SIGNAL)
	{
		__hardware_bemf_attenuation_enable();
		__hardware_bemf_blanking_mode_set_blanking_disable();
		__hardware_pwm_irq_at_end_pulse_disable();
	}
	else if(motor_ptr->bemf_sense_type& (ZCD_SENSE_TYPE_CONTINOUS_ON_TON_ONLY|
																			 ZCD_SENSE_TYPE_PUNTUAL_SAMPLE_END_TON_ONLY))
	{ //Sensados en TON
		__hardware_bemf_attenuation_enable();
		__hardware_bemf_blanking_mode_set_blanking_on_toff_only();
		__hardware_pwm_set_irq_at_end_ton_only();
	}
	else if(motor_ptr->bemf_sense_type& (ZCD_SENSE_TYPE_CONTINOUS_ON_TON_AND_TOFF|
																			 ZCD_SENSE_TYPE_PUNTUAL_SAMPLE_END_TON_AND_TOFF))
	{	//Sensados en TON y TOFF
		__hardware_bemf_attenuation_auto();
		__hardware_bemf_blanking_mode_set_blanking_disable();
		__hardware_pwm_set_irq_at_end_ton_and_toff();
	}
	else
		return -1;
	
	
	return 0;
}

int32_t zcd_sense_enable(volatile struct motor_characteristic * motor_ptr)
{
	if(motor_ptr->bemf_sense_type>=ZCD_SENSE_TYPE_COUNT)
		return -1;
	
	if(			motor_ptr->bemf_sense_type& (ZCD_SENSE_TYPE_CONTINOUS_ON_TOFF_ONLY |
																			 ZCD_SENSE_TYPE_CONTINOUS_ON_TON_ONLY  |
																			 ZCD_SENSE_TYPE_CONTINOUS_ON_TON_AND_TOFF))
	{	//Sensado continuo (Con comparador)
		__hardware_pwm_reset_irq_at_end_of_pulse();
		
		if(motor_ptr->bemf_slope == MOTOR_BEMF_SLOPE_POSITIVE)
		{
			__hardware_comp_bemf_set_falling_edge_irq_only();
		}
		else
		{			
			__hardware_comp_bemf_set_rising_edge_irq_only();
		}
		
		__hardware_comp_bemf_clear_flags();
		__hardware_comp_bemf_enable_irq();
	}
	else if( motor_ptr->bemf_sense_type& (ZCD_SENSE_TYPE_PUNTUAL_SAMPLE_END_TOFF_ONLY|
																			  ZCD_SENSE_TYPE_PUNTUAL_SAMPLE_END_TON_ONLY |
																				ZCD_SENSE_TYPE_PUNTUAL_SAMPLE_END_TON_AND_TOFF))
	{	//Sensado muestreado con flanco de GPIO al final de Ton y/o Toff
		__hardware_comp_bemf_disable_irq();
		__hardware_pwm_irq_at_end_pulse_enable();
	}
	else if( motor_ptr->bemf_sense_type & ZCD_SENSE_TYPE_CONTINUOUS_WITHOUT_BLANK_SIGNAL)
	{
		__hardware_comp_bemf_enable_irq();
	}
	
	return 0;
}


int32_t zcd_sense_disable(volatile struct motor_characteristic * motor_ptr)
{
	if(motor_ptr->bemf_sense_type>=ZCD_SENSE_TYPE_COUNT)
		return -1;
		
	__hardware_pwm_reset_irq_at_end_of_pulse();
	__hardware_comp_bemf_disable_irq();
	__hardware_comp_bemf_clear_flags();
	
	return 0;
}




int32_t set_blanking_configure (volatile struct motor_characteristic * motor_ptr,uint32_t blanking_time_us)
{
	__hardware_tim2_set_irq_in_us(blanking_time_us);
	zcd_sense_disable(&motor);
	return 0;
}



