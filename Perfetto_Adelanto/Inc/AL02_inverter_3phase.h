#ifndef AL02_INVERTER_3PHASE

#define AL02_INVERTER_3PHASE

#include "AL01_board_iq_aed_rev20191011.h"

#define INVERTER_COMM_FREWHEEL		(1<<0)/*Todos los transistores abiertos*/
#define INVERTER_COMM_SEQ1				(1<<1)
#define INVERTER_COMM_SEQ2				(1<<2)
#define INVERTER_COMM_SEQ3				(1<<3)
#define INVERTER_COMM_SEQ4				(1<<4)
#define INVERTER_COMM_SEQ5				(1<<5)
#define INVERTER_COMM_SEQ6				(1<<6)
#define INVERTER_COMM_BREAK_LOW		(1<<7)/*Todos los transistores de arriba abiertos y todos los de abajo conduciendo*/
#define	INVERTER_COMM_BREAK_HIGH	(1<<8)/*Todos los transistores de arriba conduciendo y todos los de abajo abiertos*/
#define INVERTER_COMM_COUNT				(1<<9)

#define INVERTER_STATE_OVERWRITE		0
#define INVERTER_STATE_NOT_OVERWRITE	1

#define INVERTER_COMM_DIRECTION_FOWARD	0
#define	INVERTER_COMM_DIRECTION_REVERSE	1

#define INVERTER_BEMF_SLOPE_POSITIVE	0
#define INVERTER_BEMF_SLOPE_NEGATIVE	1

#define INVERTER_BEMF_ON_OUTPUT_1		0
#define INVERTER_BEMF_ON_OUTPUT_2		1
#define INVERTER_BEMF_ON_OUTPUT_3		2


int32_t inverter_3phase_get_actual_bemf_slope			  			 (void);
int32_t inverter_3phase_get_actual_bemf_out				   			 (void);
int32_t inverter_3phase_comm_set_seq					   					 (int32_t inverter_comm_seq,int32_t inverter_state_overwrite);
int32_t	inverter_3phase_get_actual_comm_seq 			   			 (void);
int32_t inverter_3phase_comm_next_seq 			   		   			 (void);
int32_t inverter_3phase_init_config						   		 		   (void);
int32_t inverter_3phase_insert_toff_at_comm_begin_and_final(int32_t toff_us_begin,int32_t comm_period_us,int32_t toff_us_final);
int32_t inverter_3phase_get_toff_at_comm_final			   		 (void);
int32_t inverter_3phase_get_toff_at_comm_begin			   		 (void);
int32_t	inverter_3phase_get_comm_direction				   		   (void);
int32_t	inverter_3phase_set_comm_direction				   			 (int32_t inverter_direction);

#define inverter_3phase_pwm_set_sense_on_toff_before_ton_us(time_before_ton_us)		board_pwm_set_sense_on_toff_before_ton_us(time_before_ton_us)
#define inverter_3phase_pwm_set_period_us(pwm_period_us)							board_pwm_set_period_us(pwm_period_us)
#define inverter_3phase_pwm_get_period_us() 							  			((int32_t)board_pwm_get_period_us())
#define inverter_3phase_pwm_set_ton_us(pwm_ton_us)									board_pwm_set_ton_us(pwm_ton_us)
#define inverter_3phase_pwm_get_ton_us()								  			((int32_t)board_pwm_get_ton_us())
#define inverter_3phase_pwm_set_toff_us(pwm_toff_us)								board_pwm_set_toff_us(pwm_toff_us)
#define inverter_3phase_pwm_get_toff_us()								  			((int32_t)board_pwm_get_toff_us())

/* Jose */
#define inverter_3phase_pwm_get_duty_percentage()								  	{((int32_t)board_pwm_get_ton_us()); / ((int32_t)board_pwm_get_period_us());}

#endif /* AL02_INVERTER_3PHASE */



