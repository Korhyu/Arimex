#ifndef __MOTOR_TRIFASE_H

#include "stm32f0xx_hal.h"
#include "stdint.h"
#include "hardware_functions_low_level.h"

extern volatile struct motor_characteristic motor;



#define COMP_BEMF_ZCD_COMP_OUT_RISING_EDGE  1			
#define COMP_BEMF_ZCD_COMP_OUT_FALLING_EDGE 0

#define COMP_BEMF_ZCD_COMP_OUT_SEQ1_DETECTION_EDGE 				COMP_BEMF_ZCD_COMP_OUT_RISING_EDGE
#define COMP_BEMF_ZCD_COMP_OUT_SEQ3_DETECTION_EDGE 				COMP_BEMF_ZCD_COMP_OUT_RISING_EDGE
#define COMP_BEMF_ZCD_COMP_OUT_SEQ5_DETECTION_EDGE 				COMP_BEMF_ZCD_COMP_OUT_RISING_EDGE
#define COMP_BEMF_ZCD_COMP_OUT_SEQ2_DETECTION_EDGE 				COMP_BEMF_ZCD_COMP_OUT_FALLING_EDGE
#define COMP_BEMF_ZCD_COMP_OUT_SEQ4_DETECTION_EDGE 				COMP_BEMF_ZCD_COMP_OUT_FALLING_EDGE
#define COMP_BEMF_ZCD_COMP_OUT_SEQ6_DETECTION_EDGE 				COMP_BEMF_ZCD_COMP_OUT_FALLING_EDGE

#define COMP_BEMF_ZCD_INV_IN_OVER_P_IN		0
#define COMP_BEMF_ZCD_INV_IN_BELOW_P_IN		1

#define COMP_BEMF_ZCD_COMP_OUT_SEQ1_ZCD_LOST_DETECT					COMP_BEMF_ZCD_INV_IN_BELOW_P_IN
#define COMP_BEMF_ZCD_COMP_OUT_SEQ1_ZCD_NOT_DETECTED_YET		COMP_BEMF_ZCD_INV_IN_OVER_P_IN
#define COMP_BEMF_ZCD_COMP_OUT_SEQ2_ZCD_LOST_DETECT					COMP_BEMF_ZCD_INV_IN_OVER_P_IN
#define COMP_BEMF_ZCD_COMP_OUT_SEQ2_ZCD_NOT_DETECTED_YET		COMP_BEMF_ZCD_INV_IN_BELOW_P_IN
#define COMP_BEMF_ZCD_COMP_OUT_SEQ3_ZCD_LOST_DETECT					COMP_BEMF_ZCD_INV_IN_BELOW_P_IN
#define COMP_BEMF_ZCD_COMP_OUT_SEQ3_ZCD_NOT_DETECTED_YET		COMP_BEMF_ZCD_INV_IN_OVER_P_IN
#define COMP_BEMF_ZCD_COMP_OUT_SEQ4_ZCD_LOST_DETECT					COMP_BEMF_ZCD_INV_IN_OVER_P_IN
#define COMP_BEMF_ZCD_COMP_OUT_SEQ4_ZCD_NOT_DETECTED_YET		COMP_BEMF_ZCD_INV_IN_BELOW_P_IN
#define COMP_BEMF_ZCD_COMP_OUT_SEQ5_ZCD_LOST_DETECT					COMP_BEMF_ZCD_INV_IN_BELOW_P_IN
#define COMP_BEMF_ZCD_COMP_OUT_SEQ5_ZCD_NOT_DETECTED_YET		COMP_BEMF_ZCD_INV_IN_OVER_P_IN
#define COMP_BEMF_ZCD_COMP_OUT_SEQ6_ZCD_LOST_DETECT					COMP_BEMF_ZCD_INV_IN_OVER_P_IN
#define COMP_BEMF_ZCD_COMP_OUT_SEQ6_ZCD_NOT_DETECTED_YET		COMP_BEMF_ZCD_INV_IN_BELOW_P_IN

#define MOTOR_CHARGE_BOOTSTRAP_TIME_mS	500
#define MOTOR_ALGIMENT_TIME_mS 					100


#define MOTOR_STATE_STOPPED							0
#define MOTOR_STATE_STARTING						1
#define MOTOR_STATE_RUNNING							2
#define MOTOR_STATE_FAIL								3


#define MOTOR_STARTING_INIT												0
#define MOTOR_STARTING_CHARGE_BOOTSTRAP 					1
#define MOTOR_STARTING_ALIGNMENT									2
#define MOTOR_STARTING_FIRST_STEPS_FROM_STAND 		3
#define MOTOR_STARTING_FREEWHEEL_PERIOD_MEASUR		4
#define MOTOR_STARTING_FREEWHEEL_SYNC							5
#define MOTOR_STARTING_STEPS_IN_CORRECT_TIMMING		6


//--------------------------------------------
#define MOTOR_COMM_SEQ1 						0
#define MOTOR_COMM_SEQ2 						1
#define MOTOR_COMM_SEQ3 						2
#define MOTOR_COMM_SEQ4 						3
#define MOTOR_COMM_SEQ5 						4
#define MOTOR_COMM_SEQ6 						5
#define MOTOR_COMM_SEQ_COUNT				6
#define MOTOR_COMM_FREEWHEEL 				7
#define MOTOR_COMM_BREAK_LOW 				8
#define MOTOR_COMM_BREAK_HIGH  			9

#define MOTOR_COMM_STATE_OVERWRITE	0	/*Actualiza el campo motor_current_comm_seq de la estructura del motor*/
#define MOTOR_COMM_STATE_PRESERVE		1 /*Conserva  el campo motor_current_comm_seq de la estructura del motor*/
//---------------------------------------------


#define MOTOR_SPIN_DIRECTION_FOWARD		 0
#define MOTOR_SPIN_DIRECTION_REVERSE	 1

#define MOTOR_BEMF_VREF_SIGNAL_STAR_CENTER 0
#define MOTOR_BEMF_VREF_SIGNAL_MID_VBUS		 1

#define MOTOR_BEMF_SLOPE_POSITIVE 0
#define MOTOR_BEMF_SLOPE_NEGATIVE 1



#define ZCD_SENSE_TYPE_PUNTUAL_SAMPLE_END_TON_ONLY 			(1<<0)
#define ZCD_SENSE_TYPE_PUNTUAL_SAMPLE_END_TOFF_ONLY    	(1<<1)
#define ZCD_SENSE_TYPE_PUNTUAL_SAMPLE_END_TON_AND_TOFF 	(1<<2)
#define ZCD_SENSE_TYPE_CONTINOUS_ON_TON_ONLY						(1<<3)
#define ZCD_SENSE_TYPE_CONTINOUS_ON_TOFF_ONLY						(1<<4)
#define ZCD_SENSE_TYPE_CONTINOUS_ON_TON_AND_TOFF				(1<<5)
#define ZCD_SENSE_TYPE_CONTINUOUS_WITHOUT_BLANK_SIGNAL	(1<<6)
#define ZCD_SENSE_TYPE_COUNT														(1<<7)

#define BEMF_STATE_CONMUTATE								0
#define BEMF_STATE_BLANKING_INDUCTIVE_PULSE 1
#define BEMF_STATE_WAITING_ZCD							2
#define BEMF_STATE_ZCD_DETECTED 						3
#define BEMF_STATE_ZCD_LOST									4

#define PRINT_MSG_Q_LENGTH	  5
#define PRINT_MSG_PRINTED			0
#define	PRINT_MSG_NOT_PRINTED	1

#define COMM_CALLBACK_STATE_DISCHARGE 0
#define COMM_CALLBACK_STATE_COMMUTATE 1

struct motor_characteristic
{
	int32_t	 power_stage_temp_C;
	
	int32_t  vbus_voltage_V;

	int32_t  total_current_shunt;
	
	uint32_t motor_current_comm_seq;
	
	uint32_t motor_spin_direction;
	
	uint32_t bemf_state_machine;

	uint32_t time_zcd_to_zcd;
	
	uint32_t time_zcd_to_zcd_avg;
	
	int32_t time_from_comm_to_zcd;
	
	int32_t time_from_comm_to_zcd_avg;
	
	int32_t t_zcd_expected;
	
	int32_t t_error;
	
	int32_t comm_time_us;
	
	int32_t comm_time_avg_us;

	int32_t discharge_inductance_time_us;

	int32_t comm_callback_state;

	uint32_t motor_rpm_avg;						
	
	uint32_t bemf_vref_signal;											//Vbus o Star Center
	
	uint32_t bemf_slope;														//Positivo o negativo
	
	uint32_t bemf_sense_type;												//Sense on "end of toff" "toff only " "ton only" "ton y toff"
	
	uint32_t state;																	//Estado del motor: stopped,running,starting,etc
	
	uint32_t starting_state;												//Esto de la maquina de estado del arranque
	
	uint32_t starting_first_steps_count;
	
	uint32_t comp_bemf_level_lost_zcd_detect;				//almacena el nivel de salida del comparador luego de haber pasado el ZCD 
	uint32_t comp_bemf_level_zcd_not_detected_yet;	//almacena el nivel de salida del comparador antes del ZCD
	uint32_t comp_bemf_detection_edge_selection;		//almacena si la deteccion debe ser en rising o falling edge de la salida del comparador
};


void 		motor_init										(volatile struct motor_characteristic * motor_pointer);
void 		motor_task									  (volatile struct motor_characteristic * motor_ptr);
int32_t motor_driver_set_to 					(volatile struct motor_characteristic * motor_ptr, uint32_t motor_sequence, uint32_t motor_comm_state);
int32_t motor_driver_next_seq 				(volatile struct motor_characteristic * motor_ptr);

int32_t zcd_sense_enable							(volatile struct motor_characteristic * motor_ptr);
int32_t set_blanking_configure 				(volatile struct motor_characteristic * motor_ptr,uint32_t blanking_time_us);
int32_t zcd_sense_disable							(volatile struct motor_characteristic * motor_ptr);
int32_t zcd_sense_set_type						(volatile struct motor_characteristic * motor_ptr,uint32_t modo);
int32_t set_blanking_configure 				(volatile struct motor_characteristic * motor_ptr,uint32_t blanking_time_us);

void hardware_set_next_comm_seq_with_discharge_within_time(uint32_t comm_time_us,volatile struct motor_characteristic *motor_pointer);

#define __MOTOR_TRIFASE_H
#endif /* __MOTOR_TRIFASE_H */

