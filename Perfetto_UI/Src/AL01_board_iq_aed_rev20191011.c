#include "AL01_board_iq_aed_rev20191011.h"


void board_hardware_configuration (void)
{
	
	//Configuraciones de GPIO
	//GPIOAx como Salida Push Pull
	board_hardware_gpio_config_output_pp_pins_load_config  		(GPIOA, GPIO_PIN_0|GPIO_PIN_3|GPIO_PIN_11|GPIO_PIN_15);
	//GPIOAx Como "Open Drain" / HiZ
	board_hardware_gpio_config_hiz_pins_load_config		   		(GPIOA, GPIO_PIN_15_HIZ);
	//GPIOAx Como deteccion(irq) en flancos
	board_hardware_gpio_config_edge_events_pins_load_config		(GPIOA, 0);
	//GPIOAx Como entradas logicas
	board_hardware_gpio_config_inputs_pins_load_config	   		(GPIOA, GPIO_PIN_6|GPIO_PIN_8);
	
	//Idem para GPIOBx
	board_hardware_gpio_config_output_pp_pins_load_config		(GPIOB, GPIO_PIN_10|GPIO_PIN_14|GPIO_PIN_0|GPIO_PIN_13);
	board_hardware_gpio_config_hiz_pins_load_config				(GPIOB, 0);
	board_hardware_gpio_config_edge_events_pins_load_config		(GPIOB, GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_5);
	board_hardware_gpio_config_inputs_pins_load_config			(GPIOB, GPIO_PIN_12);

	//Idem para GPIOCx
	board_hardware_gpio_config_output_pp_pins_load_config		(GPIOC, GPIO_PIN_2);
	board_hardware_gpio_config_hiz_pins_load_config				(GPIOC, GPIO_PIN_3|GPIO_PIN_0);
	board_hardware_gpio_config_edge_events_pins_load_config		(GPIOC, 0);
	board_hardware_gpio_config_inputs_pins_load_config			(GPIOC, GPIO_PIN_3|GPIO_PIN_6|GPIO_PIN_7);
	
	
	micro_config();		
}



//-------------------------------------------
//				SCHEDULER/TEMPORIZADOR
//-------------------------------------------
//Calcula el tiempo futuro en el que debe ejecutarse una tarea 
//(calcula a partir de que Numero de tick debe ejecutar la tarea)
int32_t board_scheduler_load_timer			(int32_t time_ms)
{
		int32_t actual_tick;
		actual_tick=__hardware_get_tick_timer();
		actual_tick+=time_ms;
		return actual_tick;
}


//Revisa si debe ejecutarse o no una tarea, en funcion del Numero de tick actual 
//y el numero de ticks pasado por parametro
int32_t board_scheduler_is_time_expired	(int32_t timer)
{
		int32_t actual_tick;
		actual_tick=__hardware_get_tick_timer();
		actual_tick-=timer;
	
		if(actual_tick>=0)
			return 1;
		else
			return 0;
}


//-------------------------------------------
//				      	ADC 
//-------------------------------------------
int32_t board_adc_get_vbus_voltage_C (void)
{
	return ((board_adc_get_rank1()+46)/10);
}
int32_t board_adc_get_shunt_average_current_mA (void)
{
	return (board_adc_get_rank0());
}
int32_t board_adc_get_power_drivers_temp_C (void)
{
	return (board_adc_get_rank2());
}
int32_t board_adc_get_heater_temp_C (void)
{
	return (board_adc_get_rank3());
}
