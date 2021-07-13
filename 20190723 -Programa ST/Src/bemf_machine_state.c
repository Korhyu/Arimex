#include "bemf_machine_state.h"
#include "motor_trifase.h"




#define DEBUG_DISABLE 0
#define DEBUG_ENABLE  1

#define DEBUG_MODE DEBUG_DISABLE
#define CYCLES_TO_ANALYZE 10



static int32_t discharge_inductance_time_us_global=0;


static int ciclo=0;

static int32_t debug_actual_tcom		 		[(CYCLES_TO_ANALYZE*DEBUG_MODE)+1];
static int32_t debug_actual_tcom_avg 		[(CYCLES_TO_ANALYZE*DEBUG_MODE)+1];

static int32_t debug_actual_tzcdzcd  		[(CYCLES_TO_ANALYZE*DEBUG_MODE)+1];
static int32_t debug_actual_tzcdzcd_avg [(CYCLES_TO_ANALYZE*DEBUG_MODE)+1];

static int32_t debug_actual_comzcd			[(CYCLES_TO_ANALYZE*DEBUG_MODE)+1];
static int32_t debug_actual_comzcd_avg	[(CYCLES_TO_ANALYZE*DEBUG_MODE)+1];

static int32_t debug_actual_expected 		[(CYCLES_TO_ANALYZE*DEBUG_MODE)+1];
static int32_t debug_actual_error	   		[(CYCLES_TO_ANALYZE*DEBUG_MODE)+1];

static int32_t debug_next_tcom					[(CYCLES_TO_ANALYZE*DEBUG_MODE)+1];




void zcd_event(void);



static void control_loop_calculate(void)
{
				if(DEBUG_MODE)
				{
					debug_actual_tcom[ciclo] = motor.comm_time_us;
				}
				
				
				motor.time_zcd_to_zcd_avg = (motor.time_zcd_to_zcd_avg + motor.time_zcd_to_zcd)>>1; //Tomo el tiempo entre 2 flancos positivos de BEMF
				
				
				//El error se debe a la relacion entre Tcom-zcd y la duracion de 1 secuencia.
				
				motor.time_from_comm_to_zcd_avg = (motor.time_from_comm_to_zcd_avg + motor.time_from_comm_to_zcd)>>1;
				
				motor.t_zcd_expected = (motor.time_zcd_to_zcd_avg>>2)+(motor.time_zcd_to_zcd_avg>>4); //ESTO DE ACA DETERMINA DONDE TERMINARA EL ZCD EN RELACION A LA CONMUTACION
																																		 //ACA PONGO QUE TZCD=TCOM/2 ENTONCES EL CONTROL SE AJUSTARA PARA QUE CAIGA EN EL MEDIO
		
				//Voy a calcular cuanto voy a acortar la siguiente secuencia (que es sin sensar, en flanco negativo).
				//La correccion se debe a:
				//1) cuanto "de más" o "de menos" llego a conducir la ultima secuencia de flanco positivo. Dado que el periodo estaba precargado antes de
				//	 llegar a sensar ZCD y saber el periodo
				
				//2) Corrimientos en ZCD dado "ruido" (pwm-etc).
				
				motor.t_error = ((motor.time_zcd_to_zcd_avg>>1) - motor.comm_time_us);
				
				motor.t_error = motor.t_error + (motor.time_from_comm_to_zcd_avg - motor.t_zcd_expected);
								
				motor.comm_time_us = motor.time_zcd_to_zcd_avg>>1;	//Entre dos detecciones de flanco ascendente hay 2 secuencias de conmutacion.
																														//por eso divido por 2 para obtener el tiempo de 1 secuencia.



				//motor.comm_time_us = motor.comm_time_us + (motor.t_error>>1);

				motor.discharge_inductance_time_us = discharge_inductance_time_us_global;
				hardware_set_next_comm_seq_with_discharge_within_time((motor.comm_time_us+motor.t_error),&motor);//__hardware_tim3_set_irq_in_us(motor.comm_time_us+(motor.t_error));	
		
		
		
				hardware_pwm_set_time_for_sense_zcd_before_ton_starts_us(0);



				
				if(DEBUG_MODE)
				{
					
					debug_actual_tzcdzcd    [ciclo] = motor.time_zcd_to_zcd;
					debug_actual_tzcdzcd_avg[ciclo] = motor.time_zcd_to_zcd_avg;
					debug_actual_comzcd			[ciclo] = motor.time_from_comm_to_zcd;
					debug_actual_expected   [ciclo] = motor.t_zcd_expected;
					debug_actual_error		  [ciclo] = motor.t_error;
					debug_next_tcom					[ciclo] = motor.comm_time_us;
					
					if(ciclo==CYCLES_TO_ANALYZE)
					{			
						motor_driver_set_to(&motor,MOTOR_COMM_FREEWHEEL,MOTOR_COMM_STATE_OVERWRITE);
				
						for(ciclo=0;ciclo<100;ciclo++)
							printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
						
						printf("\n\n\r");
						printf("   Tcom Act  |  ZCD-ZCD | ZCD-ZCDavg | COM-ZCD | ZCD-Expec | Terror | Tcom NEXT |   N\n");
				
						for(ciclo=0;ciclo<CYCLES_TO_ANALYZE+1;ciclo++)
						{
							printf("%8d     |%8d  |%8d    |",debug_actual_tcom[ciclo], debug_actual_tzcdzcd[ciclo],debug_actual_tzcdzcd_avg[ciclo]);
						
							printf("%7d  | %8d  | %5d  |",debug_actual_comzcd[ciclo],debug_actual_expected[ciclo],debug_actual_error[ciclo]);
						
							printf("%7d    |%4d\n\r",debug_next_tcom[ciclo],ciclo);
						}
						while(1);
					}
					ciclo++;
				}
				else
				{
					if(ciclo<10)
						ciclo++;
					else
					{
								if(hardware_pwm_get_ton()<88)
								{
									ciclo=0;
									hardware_pwm_set_ton_us(hardware_pwm_get_ton()+1);
								}
					}
				}				
				
}







#define __tim3_blanking_time_calc(comm_time_us) (((comm_time_us)>>3)+((comm_time_us)>>6)) /* Esto es 0.125*Tcomm */

void commutation_event(void)
{
	
	if(motor.starting_state == MOTOR_STARTING_FIRST_STEPS_FROM_STAND)
	{	
		motor_driver_next_seq(&motor);
		
		if(motor.bemf_slope == MOTOR_BEMF_SLOPE_POSITIVE)
		{
			motor.comm_time_us = __hardware_tim3_get_count_us();
			motor.comm_time_avg_us = (((motor.comm_time_avg_us<<1)<<2) - (motor.comm_time_avg_us<<1) + motor.comm_time_us)>>2;		
			
			motor.comm_time_avg_us = motor.comm_time_avg_us >>1;	//Por eso aca hago la division /2 y antes multiplique por 2 el avg
			motor.comm_time_us = motor.comm_time_us>>1;
				
		__hardware_tim3_abort();			
		__hardware_tim3_set_irq_in_us(20000);
		}
	
		set_blanking_configure(&motor,(motor.comm_time_avg_us>>3));		
	}

	else if(motor.starting_state == MOTOR_STARTING_STEPS_IN_CORRECT_TIMMING)
	{		
		motor_driver_next_seq(&motor);
		
		if(motor.bemf_slope == MOTOR_BEMF_SLOPE_POSITIVE)
		{
			
			if(motor.bemf_state_machine != BEMF_STATE_CONMUTATE)
			{
				motor_driver_set_to(&motor,MOTOR_COMM_FREEWHEEL,MOTOR_COMM_STATE_OVERWRITE);
				printf("\n\rERROR: Esta por empezar BEMF positiva pero state machine o es BEMF CONMUTATE\n\r");
				while(1);
			}			
		
			motor.bemf_state_machine = BEMF_STATE_WAITING_ZCD;
			motor.discharge_inductance_time_us = 0;
			hardware_set_next_comm_seq_with_discharge_within_time((motor.comm_time_us-20),&motor);//__hardware_tim3_set_irq_in_us(motor.comm_time_us-12);
			
			set_blanking_configure(&motor,__tim3_blanking_time_calc(motor.comm_time_avg_us));
		}
		else
		{	//NEGATIVE SLOPE
						
				if(motor.bemf_state_machine != BEMF_STATE_ZCD_DETECTED)
				{
					
					if(hardware_pwm_get_ton()>=88 &&discharge_inductance_time_us_global<40)
					{
						discharge_inductance_time_us_global++;
					}
					
								//ZCD LOST!!
				__hardware_led_fail_sync_set();
					motor_driver_next_seq(&motor);
					motor_driver_set_to(&motor,MOTOR_COMM_FREEWHEEL,MOTOR_COMM_STATE_PRESERVE);
				__hardware_bemf_vref_select_virtual_neutral_point();
					zcd_sense_set_type(&motor,ZCD_SENSE_TYPE_CONTINUOUS_WITHOUT_BLANK_SIGNAL);
		
					motor.starting_state = MOTOR_STARTING_FREEWHEEL_PERIOD_MEASUR;
				
				__hardware_tim3_abort();
					
					motor.starting_first_steps_count=0;
				
					set_blanking_configure(&motor,motor.comm_time_avg_us>>1); //Tiro un blanking porque apenas libero el motor hay un transitorio feo que puede dar falsas detecciones ZCD			

					
				}
				else
				{
					__hardware_led_fail_sync_reset();
					motor.bemf_state_machine = BEMF_STATE_CONMUTATE;
							
				//CHECKEAR SI SE DETECTO ZCD O HAY QUE HACER CALCULOS CORRECTIVOS SOBRE ESTO.	
					control_loop_calculate();
			
					zcd_sense_disable(&motor);
				}
				//CALCULATE CLOSE LOOP
		
		}
	}	
}




void zcd_event(void)
{	
	if(motor.starting_state == MOTOR_STARTING_FIRST_STEPS_FROM_STAND)
	{
		motor.bemf_state_machine = BEMF_STATE_ZCD_DETECTED;
	
		if(motor.bemf_slope == MOTOR_BEMF_SLOPE_POSITIVE)
		{
			motor.time_from_comm_to_zcd = __hardware_tim3_get_count_us();										//En verdad se toma el tiempo entre dos ZCDs de BEMF positivo 
		}
		
		if(motor.starting_first_steps_count>0)
		{	
			if(motor.bemf_slope == MOTOR_BEMF_SLOPE_NEGATIVE)
				motor.starting_first_steps_count--;
			
			commutation_event();
		}
		else 
		{	//Pasaje a medicion de periodo en freewheel, siempre entra aca en alguna SEQx con pendiente BEMF positiva

			__hardware_io_ext_set();
			motor_driver_next_seq(&motor);
			motor_driver_next_seq(&motor);
			motor_driver_set_to(&motor,MOTOR_COMM_FREEWHEEL,MOTOR_COMM_STATE_PRESERVE);
			__hardware_bemf_vref_select_virtual_neutral_point();
			zcd_sense_set_type(&motor,ZCD_SENSE_TYPE_CONTINUOUS_WITHOUT_BLANK_SIGNAL);
	
			motor.starting_state = MOTOR_STARTING_FREEWHEEL_PERIOD_MEASUR;
			
			__hardware_tim3_abort();
						
			set_blanking_configure(&motor,200); //Tiro un blanking porque apenas libero el motor hay un transitorio feo que puede dar falsas detecciones ZCD			
		}
	}
	else if (motor.starting_state == MOTOR_STARTING_FREEWHEEL_PERIOD_MEASUR)
	{
			if(motor.starting_first_steps_count==0)
			{
				hardware_set_next_comm_seq_with_discharge_within_time(motor.comm_time_avg_us<<4,&motor);//ESTO SOLO ES PARA INICIAR EL TIMER PARA CONTAR EL TIEMPO DE CICLO!!
				__hardware_io_ext_toggle();
				motor.starting_first_steps_count++;
				motor_driver_next_seq(&motor);
				motor_driver_next_seq(&motor);
				motor_driver_set_to(&motor,MOTOR_COMM_FREEWHEEL,MOTOR_COMM_STATE_PRESERVE);
				set_blanking_configure(&motor,motor.comm_time_avg_us>>1);
			}
			else
			{		//INICIIALIZACION DE VARIABLES DE CLOSE LOOP Y SALTAR A COMM EN "X" TIEMPO DADO QUE ESTAMOS PARADOS EN ZCD EXACTAMENTE.			
				
				__hardware_io_ext_toggle();
				__hardware_tim15_set_irq_in_us(20000);
				
					motor.comm_time_us =__hardware_tim3_get_count_us();	//Esta es la medicion de un periodo electrico (osea el tiempo entre 6 secuencias)				
			
					//motor.time_zcd_to_zcd = (motor.comm_time_us>>2) + (motor.comm_time_us>>4) + (motor.comm_time_us>>6);	//Por eso divido por 3 que es el tiempo entre zcd y zcd
					motor.time_zcd_to_zcd = motor.comm_time_us;
					
				
					motor.time_zcd_to_zcd_avg = motor.time_zcd_to_zcd ;
				
					//Por eso aca "divido" por 6 para saber el tiempo de comm
					//motor.comm_time_avg_us = (motor.comm_time_us>>3) + (motor.comm_time_us>>5) + (motor.comm_time_us>>7) + (motor.comm_time_us>>9); 
					motor.comm_time_avg_us = motor.comm_time_us>>1;
						
				
					hardware_set_next_comm_seq_with_discharge_within_time(((motor.comm_time_avg_us>>1)-11),&motor);//__hardware_tim3_set_irq_in_us(((motor.comm_time_avg_us>>1)-11));//Hago la conmutacion en Tcom/2
							
					motor.comm_time_us = motor.comm_time_avg_us;
					
					motor.time_from_comm_to_zcd = motor.comm_time_avg_us>>1;
					motor.time_from_comm_to_zcd_avg = motor.time_from_comm_to_zcd;
				
					motor.t_zcd_expected = motor.comm_time_avg_us>>1;
							
				__hardware_bemf_vref_select_vbus();
				
					motor.starting_state = MOTOR_STARTING_STEPS_IN_CORRECT_TIMMING;
					
					motor.bemf_state_machine = BEMF_STATE_ZCD_DETECTED;
					zcd_sense_disable(&motor);						
			}
			
	}
	else if (motor.starting_state == MOTOR_STARTING_STEPS_IN_CORRECT_TIMMING)
	{
		motor.time_from_comm_to_zcd = __hardware_tim3_get_count_us();
		
		motor.time_zcd_to_zcd = __hardware_tim15_get_count_us();
	__hardware_tim15_abort();	

	__hardware_tim15_set_irq_in_us(20000);
		motor.bemf_state_machine = BEMF_STATE_ZCD_DETECTED;		
		zcd_sense_disable(&motor);
	}
}



//Interrupcion del comparador de BEMF: Puede ser por ZCD o por deteccion de fin del pulso inductivo
void hardware_comp_bemf_callback(void)
{	
	if( ((motor.bemf_slope == MOTOR_BEMF_SLOPE_POSITIVE) && ((EXTI->FTSR&COMP_EXTI_LINE_COMP1)!=0)) ||
		  ((motor.bemf_slope == MOTOR_BEMF_SLOPE_NEGATIVE) && ((EXTI->RTSR&COMP_EXTI_LINE_COMP1)!=0))   )
	{//Es deteccion de ZCD
		zcd_event();
	}
}





//Callback al final del Toff y Ton (un tiempo antes de que haga Ton
//Se usa este mecanismo para sensar ZCD de BEMF al final del Toff y Ton
void exti0_rising_edge_callback(void)
{
	if((HAL_COMP_GetOutputLevel(&hcomp1)>0) == motor.comp_bemf_level_lost_zcd_detect)
	{
		zcd_event(); //ZCD DETECTED!
	}	
}

//Callback en el medio de Ton(un tiempo antes de que haga Toff)
void exti0_falling_edge_callback(void)
{
	if((HAL_COMP_GetOutputLevel(&hcomp1)>0) == motor.comp_bemf_level_lost_zcd_detect)
	{
		zcd_event(); //ZCD DETECTED!
	}	
}









//Blanking TIMEOUT (timer2)
void hardware_tim_blanking_callback(void)
{		

		if(motor.starting_state != MOTOR_STARTING_STEPS_IN_CORRECT_TIMMING)
		{
		__hardware_tim2_abort();
			zcd_sense_set_type(&motor,motor.bemf_sense_type);
			zcd_sense_enable(&motor);
		}
		else if(motor.starting_state == MOTOR_STARTING_STEPS_IN_CORRECT_TIMMING)
		{			
				__hardware_tim2_abort();
					zcd_sense_set_type(&motor,motor.bemf_sense_type);
					zcd_sense_enable(&motor);
		}
}
	


//Discharge time es cuanto tiempo antes de la conmutacion se abren todos los transistores para que se descargue la inductancia del motor.
void hardware_set_next_comm_seq_with_discharge_within_time(uint32_t comm_time_us,volatile struct motor_characteristic *motor_pointer)
{
	if(motor_pointer->discharge_inductance_time_us == 0)
	{
		motor_pointer->comm_callback_state = COMM_CALLBACK_STATE_COMMUTATE;
		__hardware_tim3_set_irq_in_us(comm_time_us);
	}
	else
	{
		motor_pointer->comm_callback_state = COMM_CALLBACK_STATE_DISCHARGE;
	__hardware_tim3_set_irq_in_us(comm_time_us - motor_pointer->discharge_inductance_time_us);
	}
}

//Aca entra luego de que haya detectado BEMF ZCD 
void hardware_tim3_comm_callback(void)
{	
	__hardware_tim3_abort();
	if(motor.comm_callback_state == COMM_CALLBACK_STATE_DISCHARGE)
	{
		motor_driver_set_to(&motor,MOTOR_COMM_FREEWHEEL,MOTOR_COMM_STATE_PRESERVE);
		motor.comm_callback_state = COMM_CALLBACK_STATE_COMMUTATE;
	__hardware_tim3_set_irq_in_us(motor.discharge_inductance_time_us);
	}
	else
	{
		commutation_event();
	}
}


void hardware_tim15_watchdog_callback(void)
{
	printf("Se pudrio la fruta");
	motor.state = MOTOR_STATE_STOPPED;
	__hardware_tim15_abort();
}

