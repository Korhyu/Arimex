#ifndef AL03_MOTOR_3PHASE_DRIVER_H_

#define AL03_MOTOR_3PHASE_DRIVER_H_

#include "AL02_inverter_3phase.h"

#define MOTOR_STATE_STOPPED		0
#define MOTOR_STATE_STARTING	1
#define MOTOR_STATE_RUNNING		2
#define MOTOR_STATE_FAIL		3

#define MOTOR_FAIL_TIMEOUT_1	-1
#define MOTOR_FAIL_TIMEOUT_2	-2
#define MOTOR_FAIL_TIMEOUT_3	-3

#define MOTOR_STOP_METHOD_BREAK			0
#define MOTOR_STOP_METHOD_FREEWHEEL		1

#define MOTOR_DIRECTION_FOWARD	INVERTER_COMM_DIRECTION_FOWARD
#define MOTOR_DIRECTION_REVERSE INVERTER_COMM_DIRECTION_REVERSE

int32_t  motor_3phase_init 						(void);
void	 motor_3phase_task						(void);

#define motor_3phase_get_motor_direction()						inverter_3phase_get_comm_direction()
int32_t motor_3phase_set_motor_direction(int32_t motor_direction);

int32_t  motor_3phase_start_motor				(void);
int32_t  motor_3phase_stop_motor				(int32_t motor_stop_method);
int32_t  motor_3phase_get_state					(void);

int32_t  motor_3phase_get_electrical_frequency_hz	(void);

#define motor_3phase_get_pwm_period_us()		inverter_3phase_pwm_get_period_us()
#define motor_3phase_get_pwm_ton_us				inverter_3phase_pwm_get_ton_us()


int32_t motor_3phase_set_pwm_ton_us_set_point(int32_t pwm_ton_set_point);
int32_t motor_3phase_get_pwm_ton_us_max_set_point(void);
int32_t motor_3phase_get_pwm_ton_us_min_set_point(void);
int32_t motor_3phase_get_pwm_ton_us_set_point(void);

int32_t bemf_get_fail_sync_rate_ppm (void);

void calculate_times (void);

void motor_3phase_starting_state_machine(void);

struct motor_3phase_drive{
	int32_t motor_state;

	int32_t starting_state;

	int32_t starting_sub_state;

	int32_t starting_first_steps_counter;

	int32_t bemf_sense_type;

	int32_t bemf_state;

	int32_t stop_running_flag;

	int32_t motor_electrical_period_us_avg;
	int32_t motor_comm_seq_period_us;			//Tiempo de conmutacion fija
	int32_t motor_comm_seq_period_us_avg;
	int32_t time_from_comm_to_zcd;
	int32_t time_from_comm_to_zcd_avg;
	int32_t time_from_zcd_to_zcd;
	int32_t time_from_zcd_to_zcd_avg;
	int32_t time_zcd_expected;
	int32_t time_t_error;
	int32_t time_var_comm;						//Tiempo de conmutacion variable (con el error)
	int32_t time_advance;						//Tiempo de adelanto
	int32_t time_update;						//Flag para control de actualizacion de tiempos.

	int32_t zcd_blanking_time;

	int32_t pwm_ton_us_set_point;

	int32_t fail_sync_rate_enable_flag;
	int32_t fail_sync_rate_comm_counter;
	int32_t fail_sync_rate_fails_counter;
	int32_t fail_sync_ppm;

	int32_t seq_actual;
	int32_t phase_next_zcd;


};

#endif /* AL03_MOTOR_3PHASE_DRIVER_H_ */
