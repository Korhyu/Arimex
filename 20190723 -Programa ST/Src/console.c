#include "console.h"

extern volatile uint32_t * adc_results;

extern uint32_t adc_channels_count;

extern volatile struct motor_characteristic motor;

static int32_t timer_console_print,timer_console_read;



void console_init(void)
{
		timer_console_print = scheduler_next_time_exec(CONSOLE_TIME_TO_UPDATE_SCREEN_MS);
		timer_console_read  = scheduler_next_time_exec(CONSOLE_TIME_TO_READ_UART_MS);
}

void console_task(void)
{
		static uint8_t dato;
		uint8_t aux;
		if(scheduler_is_time_to_exec(timer_console_read))
		{
			timer_console_read = scheduler_next_time_exec(CONSOLE_TIME_TO_READ_UART_MS);
			aux=hardware_uart_read();
			if(aux!=HAL_ERROR && aux!=HAL_TIMEOUT)
			{
				dato = aux;
			}	
		}
		if(scheduler_is_time_to_exec(timer_console_print))
		{
			
			
			timer_console_print = scheduler_next_time_exec(CONSOLE_TIME_TO_UPDATE_SCREEN_MS);
			printf("\n\n\n\n\n\n\n\n\n\n\n\n");
			printf("DET-SPEED:    %d\n",adc_results[1]);
			printf("NTC_SENSE:		 %d\n",motor.power_stage_temp_C);
			printf("CURR SUM:		 %d\n",motor.total_current_shunt);
			printf("VBUS:				 %d\n",motor.vbus_voltage_V);		
			printf("RPM: 				 %d\n",motor.motor_rpm_avg);
			printf("ECO de Uart -RX:%d\n",dato);
		}

	
}


