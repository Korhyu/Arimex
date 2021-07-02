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

#endif /* AL03_MOTOR_3PHASE_DRIVER_H_ */
