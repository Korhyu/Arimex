#include "AL03_motor_3phase_driver.h"



/*******************************************************************************
 * Funcion que imprime los estados de la estructura motor_3phase_drive
********************************************************************************/
void printStatus (struct motor_3phase_drive* ms)
{	
	printf("Valores de la estructura motor_3phase_drive\n");
	printf("motor_state: %d\n", ms->motor_state);
	printf("starting_state: %d\n", ms->starting_state);
	printf("starting_sub_state: %d\n", ms->starting_sub_state);
	printf("starting_first_steps_counter: %d\n", ms->starting_first_steps_counter);
	printf("bemf_sense_type: %d\n", ms->bemf_sense_type);
	printf("bemf_state: %d\n", ms->bemf_state);
	printf("stop_running_flag: %d\n", ms->stop_running_flag);
	printf("motor_electrical_period_us_avg: %d\n", ms->motor_electrical_period_us_avg);
	printf("motor_comm_seq_period_us: %d\n", ms->motor_comm_seq_period_us);
	printf("motor_comm_seq_period_us_avg: %d\n", ms->motor_comm_seq_period_us_avg);
	printf("time_from_comm_to_zcd: %d\n", ms->time_from_comm_to_zcd);
	printf("time_from_comm_to_zcd_avg: %d\n", ms->time_from_comm_to_zcd_avg);
	printf("time_from_zcd_to_zcd: %d\n", ms->time_from_zcd_to_zcd);
	printf("time_from_zcd_to_zcd_avg: %d\n", ms->time_from_zcd_to_zcd_avg);
	printf("time_zcd_expected: %d\n", ms->time_zcd_expected);
	printf("time_t_error: %d\n", ms->time_t_error);
	printf("pwm_ton_us_set_point: %d\n", ms->pwm_ton_us_set_point);
	printf("fail_sync_rate_enable_flag: %d\n", ms->fail_sync_rate_enable_flag);
	printf("fail_sync_rate_comm_counter: %d\n", ms->fail_sync_rate_comm_counter);
	printf("fail_sync_rate_fails_counter: %d\n", ms->fail_sync_rate_fails_counter);
	printf("fail_sync_ppm: %d\n", ms->fail_sync_ppm);
}