#include "AL01_board_iq_aed_rev20191011.h"


void board_hardware_configuration (void)
{
	//Configuraciones de GPIO
	//GPIOAx como Salida Push Pull
	board_hardware_gpio_config_output_pp_pins_load_config  		(GPIOA, GPIO_PIN_0|GPIO_PIN_3|GPIO_PIN_11);
	//GPIOAx Como "Open Drain" / HiZ
	board_hardware_gpio_config_hiz_pins_load_config		   		(GPIOA, GPIOA_PIN_15_HIZ|GPIOA_PIN_6_HIZ);
	//GPIOAx Como deteccion(irq) en flancos
	board_hardware_gpio_config_edge_events_pins_load_config		(GPIOA, 0);
	//GPIOAx Como entradas logicas
	board_hardware_gpio_config_inputs_pins_load_config	   		(GPIOA, GPIOA_PIN_8_IN_PULLUP);
	
	//Idem para GPIOBx
	board_hardware_gpio_config_output_pp_pins_load_config		(GPIOB, GPIO_PIN_2|GPIO_PIN_6|GPIO_PIN_11);
	board_hardware_gpio_config_hiz_pins_load_config				(GPIOB, GPIOB_PIN_10_HIZ|GPIOB_PIN_14_HIZ|GPIOB_PIN_0_HIZ|GPIOB_PIN_13_HIZ);
	//board_hardware_gpio_config_edge_events_pins_load_config	(GPIOB,0);
	board_hardware_gpio_config_edge_events_pins_load_config		(GPIOB, GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_5);
	board_hardware_gpio_config_inputs_pins_load_config			(GPIOB, GPIOB_PIN_12_IN_PULLUP);

	//Idem para GPIOCx
	board_hardware_gpio_config_output_pp_pins_load_config		(GPIOC, GPIO_PIN_2);
	board_hardware_gpio_config_hiz_pins_load_config				(GPIOC, GPIOC_PIN_3_HIZ|GPIOC_PIN_0_HIZ);
	board_hardware_gpio_config_edge_events_pins_load_config		(GPIOC, 0);
	board_hardware_gpio_config_inputs_pins_load_config			(GPIOC, GPIOC_PIN_6_IN_PULLUP|GPIOC_PIN_7_IN_PULLUP	);
	
	
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
