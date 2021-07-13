#include "measurements.h"

extern volatile struct motor_characteristic motor;

static int32_t timer_measurements;


extern volatile uint32_t * adc_results;

extern uint32_t adc_channels_count;


void measurements_init(void)
{
		timer_measurements = scheduler_next_time_exec(MEASUREMENTS_SAMPLE_TIME_MS);
}

void measurements_task(void)
{
	uint32_t aux;
	
		if(scheduler_is_time_to_exec(timer_measurements))
		{			
			timer_measurements = scheduler_next_time_exec(MEASUREMENTS_SAMPLE_TIME_MS);
	
			motor.vbus_voltage_V = (adc_results[3] + 46)/10;
			
			aux = adc_results[2];
					
			if(aux<=3700)		
				motor.power_stage_temp_C= (37683-(aux*9))>>8;
			else
				motor.power_stage_temp_C= (63808-(aux*17))>>7;
			
			aux = adc_results[0];
			
			motor.total_current_shunt = ((motor.total_current_shunt<<6) - motor.total_current_shunt + aux)>>6;
			
			motor.motor_rpm_avg = 10000000/motor.comm_time_us;
		}
}


