#include "AL02_switches_leds_mux_driver.h"

#define ui_mux_outputs_state_init()		{__hardware_gpio_output_reset(BOARD_UI_LED_ENABLE1a_PORT,BOARD_UI_LED_ENABLE1a_PIN);\
										 __hardware_gpio_output_reset(BOARD_UI_LED_ENABLE1b_PORT,BOARD_UI_LED_ENABLE1b_PIN);\
										 __hardware_gpio_output_reset(BOARD_UI_LED_ENABLE2a_PORT,BOARD_UI_LED_ENABLE2a_PIN);\
										 __hardware_gpio_output_reset(BOARD_UI_LED_ENABLE2b_PORT,BOARD_UI_LED_ENABLE2b_PIN);\
										 __hardware_gpio_output_set(BOARD_UI_SWITCH_ENABLE_PORT,BOARD_UI_SWITCH_ENABLE_PIN);}

#define ui_mux_leds_comm_enable()		{AFIO_GPxConfig(GPIO_PA, BOARD_UI_LED_ENABLE2a_PIN, 0);\
										 AFIO_GPxConfig(GPIO_PB, BOARD_UI_LED_ENABLE1a_PIN, 1);\
										 AFIO_GPxConfig(GPIO_PB, BOARD_UI_LED_ENABLE1b_PIN, 1);\
										 AFIO_GPxConfig(GPIO_PB, BOARD_UI_LED_ENABLE2b_PIN, 0);\
										 __hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_LED_ENABLE1a_PORT,BOARD_UI_LED_ENABLE1a_PIN);\
										 __hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_LED_ENABLE1b_PORT,BOARD_UI_LED_ENABLE1b_PIN);\
										 __hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_LED_ENABLE2a_PORT,BOARD_UI_LED_ENABLE2a_PIN);\
										 __hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_LED_ENABLE2b_PORT,BOARD_UI_LED_ENABLE2b_PIN);}

#define ui_mux_leds_comm_disable() 	   {__hardware_gpio_config_set_as_hiz_pin(BOARD_UI_LED_ENABLE1a_PORT,BOARD_UI_LED_ENABLE1a_PIN);\
										 __hardware_gpio_config_set_as_hiz_pin(BOARD_UI_LED_ENABLE1b_PORT,BOARD_UI_LED_ENABLE1b_PIN);\
										 __hardware_gpio_config_set_as_hiz_pin(BOARD_UI_LED_ENABLE2a_PORT,BOARD_UI_LED_ENABLE2a_PIN);\
										 __hardware_gpio_config_set_as_hiz_pin(BOARD_UI_LED_ENABLE2b_PORT,BOARD_UI_LED_ENABLE2b_PIN);}
	
#define ui_mux_leds_comm_set_high()		{__hardware_gpio_output_set(BOARD_UI_LED_ENABLE1a_PORT,BOARD_UI_LED_ENABLE1a_PIN);\
										 __hardware_gpio_output_set(BOARD_UI_LED_ENABLE1b_PORT,BOARD_UI_LED_ENABLE1b_PIN);\
										 __hardware_gpio_output_set(BOARD_UI_LED_ENABLE2a_PORT,BOARD_UI_LED_ENABLE2a_PIN);\
										 __hardware_gpio_output_set(BOARD_UI_LED_ENABLE2b_PORT,BOARD_UI_LED_ENABLE2b_PIN);}

#define ui_mux_leds_comm_set_low()		{__hardware_gpio_output_reset(BOARD_UI_LED_ENABLE1a_PORT,BOARD_UI_LED_ENABLE1a_PIN);\
										 __hardware_gpio_output_reset(BOARD_UI_LED_ENABLE1b_PORT,BOARD_UI_LED_ENABLE1b_PIN);\
										 __hardware_gpio_output_reset(BOARD_UI_LED_ENABLE2a_PORT,BOARD_UI_LED_ENABLE2a_PIN);\
										 __hardware_gpio_output_reset(BOARD_UI_LED_ENABLE2b_PORT,BOARD_UI_LED_ENABLE2b_PIN);}

#define ui_mux_switches_comm_set_low()	{__hardware_gpio_output_reset(BOARD_UI_SWITCH_ENABLE_PORT,BOARD_UI_SWITCH_ENABLE_PIN);}

#define ui_mux_switches_comm_set_high()	{__hardware_gpio_output_set(BOARD_UI_SWITCH_ENABLE_PORT,BOARD_UI_SWITCH_ENABLE_PIN);}
																				
#define ui_mux_switches_comm_enable()	{__hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_SWITCH_ENABLE_PORT,BOARD_UI_SWITCH_ENABLE_PIN);\
										 __hardware_gpio_output_set(BOARD_UI_SWITCH_ENABLE_PORT,BOARD_UI_SWITCH_ENABLE_PIN);} 

#define ui_mux_switches_comm_disable() 	{__hardware_gpio_config_set_as_hiz_pin(BOARD_UI_SWITCH_ENABLE_PORT,BOARD_UI_SWITCH_ENABLE_PIN);\
										 __hardware_gpio_output_reset(BOARD_UI_SWITCH_ENABLE_PORT,BOARD_UI_SWITCH_ENABLE_PIN);}

#define ui_mux_io_bus_set_as_output()	{__hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_IO_1_PORT,BOARD_UI_IO_1_PIN);\
										 __hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_IO_2_PORT,BOARD_UI_IO_2_PIN);\
										 __hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_IO_3_PORT,BOARD_UI_IO_3_PIN);\
										 __hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_IO_4_PORT,BOARD_UI_IO_4_PIN);\
										 __hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_IO_5_PORT,BOARD_UI_IO_5_PIN);\
										 __hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_IO_6_PORT,BOARD_UI_IO_6_PIN);}

//#define ui_mux_io_bus_set_as_output()	{AFIO_GPxConfig(GPIO_PC, BOARD_UI_IO_1_PIN, 0);\
										 AFIO_GPxConfig(GPIO_PC, BOARD_UI_IO_2_PIN, 0);\
										 AFIO_GPxConfig(GPIO_PA, BOARD_UI_IO_3_PIN, 0);\
										 AFIO_GPxConfig(GPIO_PB, BOARD_UI_IO_4_PIN, 1);\
										 GPIO_DirectionConfig(BOARD_UI_IO_1_PORT, BOARD_UI_IO_1_PIN, GPIO_DIR_OUT);\
										 GPIO_DirectionConfig(BOARD_UI_IO_2_PORT, BOARD_UI_IO_2_PIN, GPIO_DIR_OUT);\
										 GPIO_DirectionConfig(BOARD_UI_IO_3_PORT, BOARD_UI_IO_3_PIN, GPIO_DIR_OUT);\
										 GPIO_DirectionConfig(BOARD_UI_IO_4_PORT, BOARD_UI_IO_4_PIN, GPIO_DIR_OUT);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_1_PORT, BOARD_UI_IO_1_PIN, GPIO_PR_DISABLE);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_2_PORT, BOARD_UI_IO_2_PIN, GPIO_PR_DISABLE);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_3_PORT, BOARD_UI_IO_3_PIN, GPIO_PR_DISABLE);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_4_PORT, BOARD_UI_IO_3_PIN, GPIO_PR_DISABLE);\
										 GPIO_InputConfig(BOARD_UI_IO_1_PORT, BOARD_UI_IO_1_PIN, DISABLE);\
										 GPIO_InputConfig(BOARD_UI_IO_2_PORT, BOARD_UI_IO_2_PIN, DISABLE);\
										 GPIO_InputConfig(BOARD_UI_IO_3_PORT, BOARD_UI_IO_3_PIN, DISABLE);\
										 GPIO_InputConfig(BOARD_UI_IO_4_PORT, BOARD_UI_IO_4_PIN, DISABLE);}

										

//#define ui_mux_io_bus_set_as_input()	{AFIO_GPxConfig(GPIO_PC, BOARD_UI_IO_1_PIN, 0);\
										 AFIO_GPxConfig(GPIO_PC, BOARD_UI_IO_2_PIN, 0);\
										 AFIO_GPxConfig(GPIO_PA, BOARD_UI_IO_3_PIN, 0);\
										 AFIO_GPxConfig(GPIO_PB, BOARD_UI_IO_4_PIN, 1);\
										 GPIO_DirectionConfig(BOARD_UI_IO_1_PORT, BOARD_UI_IO_1_PIN, GPIO_DIR_IN);\
										 GPIO_DirectionConfig(BOARD_UI_IO_2_PORT, BOARD_UI_IO_2_PIN, GPIO_DIR_IN);\
										 GPIO_DirectionConfig(BOARD_UI_IO_3_PORT, BOARD_UI_IO_3_PIN, GPIO_DIR_IN);\
										 GPIO_DirectionConfig(BOARD_UI_IO_4_PORT, BOARD_UI_IO_4_PIN, GPIO_DIR_IN);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_1_PORT, BOARD_UI_IO_1_PIN, GPIO_PR_DOWN);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_2_PORT, BOARD_UI_IO_2_PIN, GPIO_PR_DOWN);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_3_PORT, BOARD_UI_IO_3_PIN, GPIO_PR_DOWN);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_4_PORT, BOARD_UI_IO_3_PIN, GPIO_PR_DOWN);\
										 GPIO_InputConfig(BOARD_UI_IO_1_PORT, BOARD_UI_IO_1_PIN, ENABLE);\
										 GPIO_InputConfig(BOARD_UI_IO_2_PORT, BOARD_UI_IO_2_PIN, ENABLE);\
										 GPIO_InputConfig(BOARD_UI_IO_3_PORT, BOARD_UI_IO_3_PIN, ENABLE);\
										 GPIO_InputConfig(BOARD_UI_IO_4_PORT, BOARD_UI_IO_4_PIN, ENABLE);}

#define ui_mux_io_bus_set_as_input()	{__hardware_gpio_config_set_as_hiz_pin(BOARD_UI_IO_1_PORT,BOARD_UI_IO_1_PIN);\
										 __hardware_gpio_config_set_as_hiz_pin(BOARD_UI_IO_2_PORT,BOARD_UI_IO_2_PIN);\
										 __hardware_gpio_config_set_as_hiz_pin(BOARD_UI_IO_3_PORT,BOARD_UI_IO_3_PIN);\
										 __hardware_gpio_config_set_as_hiz_pin(BOARD_UI_IO_4_PORT,BOARD_UI_IO_4_PIN);\
										 __hardware_gpio_config_set_as_hiz_pin(BOARD_UI_IO_5_PORT,BOARD_UI_IO_5_PIN);\
										 __hardware_gpio_config_set_as_hiz_pin(BOARD_UI_IO_6_PORT,BOARD_UI_IO_6_PIN);\
										 GPIO_InputConfig(BOARD_UI_IO_1_PORT, BOARD_UI_IO_1_PIN, ENABLE);\
										 GPIO_InputConfig(BOARD_UI_IO_2_PORT, BOARD_UI_IO_2_PIN, ENABLE);\
										 GPIO_InputConfig(BOARD_UI_IO_3_PORT, BOARD_UI_IO_3_PIN, ENABLE);\
										 GPIO_InputConfig(BOARD_UI_IO_4_PORT, BOARD_UI_IO_4_PIN, ENABLE);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_1_PORT, BOARD_UI_IO_1_PIN, GPIO_PR_DOWN);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_2_PORT, BOARD_UI_IO_2_PIN, GPIO_PR_DOWN);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_3_PORT, BOARD_UI_IO_3_PIN, GPIO_PR_DOWN);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_4_PORT, BOARD_UI_IO_3_PIN, GPIO_PR_DOWN);}

#define config_gpio_switch_pulldown()	{GPIO_PullResistorConfig(BOARD_UI_IO_1_PORT, BOARD_UI_IO_1_PIN, GPIO_PR_DOWN);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_2_PORT, BOARD_UI_IO_2_PIN, GPIO_PR_DOWN);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_3_PORT, BOARD_UI_IO_3_PIN, GPIO_PR_DOWN);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_4_PORT, BOARD_UI_IO_4_PIN, GPIO_PR_DOWN);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_5_PORT, BOARD_UI_IO_5_PIN, GPIO_PR_DOWN);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_6_PORT, BOARD_UI_IO_6_PIN, GPIO_PR_DOWN);}

#define config_gpio_switch_pullup()		{GPIO_PullResistorConfig(BOARD_UI_IO_1_PORT, BOARD_UI_IO_1_PIN, GPIO_PR_UP);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_2_PORT, BOARD_UI_IO_2_PIN, GPIO_PR_UP);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_3_PORT, BOARD_UI_IO_3_PIN, GPIO_PR_UP);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_4_PORT, BOARD_UI_IO_4_PIN, GPIO_PR_UP);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_5_PORT, BOARD_UI_IO_5_PIN, GPIO_PR_UP);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_6_PORT, BOARD_UI_IO_6_PIN, GPIO_PR_UP);}

#define config_gpio_switch_pull_disable()	{GPIO_PullResistorConfig(BOARD_UI_IO_1_PORT, BOARD_UI_IO_1_PIN, GPIO_PR_DISABLE);\
											 GPIO_PullResistorConfig(BOARD_UI_IO_2_PORT, BOARD_UI_IO_2_PIN, GPIO_PR_DISABLE);\
											 GPIO_PullResistorConfig(BOARD_UI_IO_3_PORT, BOARD_UI_IO_3_PIN, GPIO_PR_DISABLE);\
											 GPIO_PullResistorConfig(BOARD_UI_IO_4_PORT, BOARD_UI_IO_4_PIN, GPIO_PR_DISABLE);\
											 GPIO_PullResistorConfig(BOARD_UI_IO_5_PORT, BOARD_UI_IO_5_PIN, GPIO_PR_DISABLE);\
											 GPIO_PullResistorConfig(BOARD_UI_IO_6_PORT, BOARD_UI_IO_6_PIN, GPIO_PR_DISABLE);}

#define ui_led_max_fan_set_on()			__hardware_gpio_output_set	(BOARD_UI_IO_6_PORT,BOARD_UI_IO_6_PIN)
#define ui_led_max_fan_set_off()		__hardware_gpio_output_reset(BOARD_UI_IO_6_PORT,BOARD_UI_IO_6_PIN)
#define ui_led_mid_fan_set_on()			__hardware_gpio_output_set	(BOARD_UI_IO_5_PORT,BOARD_UI_IO_5_PIN)
#define ui_led_mid_fan_set_off()		__hardware_gpio_output_reset(BOARD_UI_IO_5_PORT,BOARD_UI_IO_5_PIN)
#define ui_led_min_fan_set_on()			__hardware_gpio_output_set	(BOARD_UI_IO_4_PORT,BOARD_UI_IO_4_PIN)
#define ui_led_min_fan_set_off()		__hardware_gpio_output_reset(BOARD_UI_IO_4_PORT,BOARD_UI_IO_4_PIN)
		
#define ui_led_max_heat_set_on()		__hardware_gpio_output_set	(BOARD_UI_IO_3_PORT,BOARD_UI_IO_3_PIN)
#define ui_led_max_heat_set_off()		__hardware_gpio_output_reset(BOARD_UI_IO_3_PORT,BOARD_UI_IO_3_PIN)
#define ui_led_mid_heat_set_on()		__hardware_gpio_output_set	(BOARD_UI_IO_2_PORT,BOARD_UI_IO_2_PIN)
#define ui_led_mid_heat_set_off()		__hardware_gpio_output_reset(BOARD_UI_IO_2_PORT,BOARD_UI_IO_2_PIN)
#define ui_led_min_heat_set_on()		__hardware_gpio_output_set	(BOARD_UI_IO_1_PORT,BOARD_UI_IO_1_PIN)
#define ui_led_min_heat_set_off()		__hardware_gpio_output_reset(BOARD_UI_IO_1_PORT,BOARD_UI_IO_1_PIN)		

#define ui_switch_coldshot_get_state()	__hardware_gpio_input_read_state(BOARD_UI_IO_6_PORT,BOARD_UI_IO_6_PIN)
#define ui_switch_lock_get_state()		__hardware_gpio_input_read_state(BOARD_UI_IO_5_PORT,BOARD_UI_IO_5_PIN)
#define ui_switch_inc_fan_get_state()	__hardware_gpio_input_read_state(BOARD_UI_IO_4_PORT,BOARD_UI_IO_4_PIN)
#define ui_switch_dec_fan_get_state()	__hardware_gpio_input_read_state(BOARD_UI_IO_3_PORT,BOARD_UI_IO_3_PIN)																				
#define ui_switch_inc_heat_get_state()	__hardware_gpio_input_read_state(BOARD_UI_IO_2_PORT,BOARD_UI_IO_2_PIN)						
#define ui_switch_dec_heat_get_state()	__hardware_gpio_input_read_state(BOARD_UI_IO_1_PORT,BOARD_UI_IO_1_PIN)		

#define ui_timer_set_irq_within_us(time_us)	board_tim_sctm_init_timer_with_timeout_irq_us(BOARD_TIM_SCTM_UI,time_us)

#define MUX_TIME_LEDS_uS			4000
#define MUX_TIME_SWITCHES_us	 	1000

#define UI_STATE_RESET				0
#define UI_STATE_LEDS				1
#define UI_STATE_SWITCHES			2

#define COUNT_DEBOUNCE				20				//Contador antirebote - Es la cantidad de veces que debo pasar por los switches en el mismo estado para que se considere presionado el mismo




volatile static uint8_t switches_state=0,leds_state=0x00;

static bool switches_change_flag = FALSE;					//Flag de que los switches tienen un nuevo valor para revisar

/*
 * Esta funcion retorna el ultimo sensado de los switches
 */
uint8_t ui_mux_get_switches_state (void)
{
	return switches_state;
}

/*
 * Esta funcion retorna el estado de los leds: usar los shift_mask para encuestar cada led
 */
uint8_t ui_mux_get_leds_state(void)
{
	return leds_state;
}

/*
 * Inicializacion de tareas y linkeo de callbacks
 */
void ui_mux_init (void)
{
	ui_mux_outputs_state_init();
	board_timer_link_callback(ui_mux_driver_routine,BOARD_TIM_UI_SEL_CALLBACK);
	ui_mux_driver_routine();
}


/*
 * Setea el valor de la variable leds. Para observar el cambio es necesario estar llamando a "ui_mux_leds_state_update"
 */
void ui_mux_set_leds_state			(uint8_t led_state)
{
	leds_state = led_state;
	ui_mux_leds_state_update();
}

/*
 * Actualiza los registros del puerto que encienden los leds (debe llamarse periodicamente como task)
 */
void	ui_mux_leds_state_update (void)
{
	if(leds_state&(1<<UI_LED_MIN_HEAT_SHIFT_MASK))
		ui_led_min_heat_set_on();
	else
		ui_led_min_heat_set_off();
	
	if(leds_state&(1<<UI_LED_MID_HEAT_SHIFT_MASK))
		ui_led_mid_heat_set_on();
	else
		ui_led_mid_heat_set_off();
	
	if(leds_state&(1<<UI_LED_MAX_HEAT_SHIFT_MASK))
		ui_led_max_heat_set_on();
	else
		ui_led_max_heat_set_off();


	if(leds_state&(1<<UI_LED_MIN_FAN_SHIFT_MASK))
		ui_led_min_fan_set_on();
	else
		ui_led_min_fan_set_off();
	
	if(leds_state&(1<<UI_LED_MID_FAN_SHIFT_MASK))
		ui_led_mid_fan_set_on();
	else
		ui_led_mid_fan_set_off();
	
	if(leds_state&(1<<UI_LED_MAX_FAN_SHIFT_MASK))
		ui_led_max_fan_set_on();
	else
		ui_led_max_fan_set_off();
}


/*
 * Esta funcion se encarga de multiplexar leds y switches compartidos en el mismo GPIO. 
 * Debe llamarse periodicamente como un callback de una irq de timer. 
 */
void ui_mux_driver_routine (void)
{
	static uint8_t actual_state=UI_STATE_RESET;
	static uint8_t state_count = 0;
	uint8_t switches=0, switches_prev=0;
	
	/*
	switch(actual_state)
	{
		case UI_STATE_RESET:		ui_mux_switches_comm_disable();
														ui_mux_leds_state_update();
		
		case UI_STATE_SWITCHES:	ui_timer_set_irq_within_us(MUX_TIME_SWITCHES_us);
															
														ui_mux_leds_comm_disable();				//Pone en Hiz los pines bajos de los LEDS
														ui_mux_leds_comm_set_high();			//Pone en alto los pines bajos de los leds para que no enciendan al pulsar el pulsador
														ui_mux_io_bus_set_as_input();			//Pone en Hiz los pines intermedios (los que estan despues de los SW)
														config_gpio_switch_pulldown();			//Pone una Rpull down en los pines de entrada
														ui_mux_switches_comm_enable();			//Pone como push-pull el pin del UI_SW_ENABLE y su estado e SET

														actual_state = UI_STATE_LEDS;		
														break;	
		
		case UI_STATE_LEDS:		ui_timer_set_irq_within_us(MUX_TIME_LEDS_uS);
															
														switches = 	(ui_switch_coldshot_get_state()	<<	UI_SWITCH_COLDSHOT_SHIFT_MASK)  +
																	(ui_switch_lock_get_state()		<<	UI_SWITCH_LOCK_SHIFT_MASK)		+
																	(ui_switch_inc_fan_get_state()	<<	UI_SWITCH_INC_FAN_SHIFT_MASK)	+
																	(ui_switch_dec_fan_get_state()	<<	UI_SWITCH_DEC_FAN_SHIFT_MASK)	+
																	(ui_switch_inc_heat_get_state()	<<	UI_SWITCH_INC_HEAT_SHIFT_MASK)  +
																	(ui_switch_dec_heat_get_state()	<<	UI_SWITCH_DEC_HEAT_SHIFT_MASK);

														switches_state = switches;				//Actualizo variable global de intercambio
		
														ui_mux_switches_comm_disable();			//Pone como Hiz el pin del UI_SW_ENABLE y su estado en RESET
														config_gpio_switch_pull_disable();		//Desabilito la R de pull down
														ui_mux_io_bus_set_as_output();			//Pone en push-pull los pines intermedios (los que estan despues de los SW)
														ui_mux_leds_comm_set_low();				//Pone en reset el estado de los pines bajos de los leds
														ui_mux_leds_comm_enable();				//Pone en push-pull los pines bajos de los LEDS

														actual_state = UI_STATE_SWITCHES;
														break;
	}
	*/

	switch(actual_state)
	{
		case UI_STATE_RESET:	
			ui_mux_switches_comm_disable();
			ui_mux_leds_state_update();
		
		case UI_STATE_SWITCHES:
			//En este estado leo los switches y luego dejo los LEDS listos
			ui_timer_set_irq_within_us(MUX_TIME_LEDS_uS);		//Cargo el tiempo que quiero que este el estado de LEDS
			
			//Leo el estado de los switches
			switches_state = 	(ui_switch_coldshot_get_state()	<<	UI_SWITCH_COLDSHOT_SHIFT_MASK)  |
								(ui_switch_lock_get_state()		<<	UI_SWITCH_LOCK_SHIFT_MASK)		|
								(ui_switch_inc_fan_get_state()	<<	UI_SWITCH_INC_FAN_SHIFT_MASK)	|
								(ui_switch_dec_fan_get_state()	<<	UI_SWITCH_DEC_FAN_SHIFT_MASK)	|
								(ui_switch_inc_heat_get_state()	<<	UI_SWITCH_INC_HEAT_SHIFT_MASK)  |
								(ui_switch_dec_heat_get_state()	<<	UI_SWITCH_DEC_HEAT_SHIFT_MASK);
			
			ui_mux_switches_comm_set_low();
			ui_mux_io_bus_set_as_output();			//Pone en push-pull los pines intermedios (lectura)
			
			ui_mux_leds_comm_set_low();
			
			actual_state = UI_STATE_LEDS;		
			break;	
		
		case UI_STATE_LEDS:
			//En este estado leo los LEDs y luego dejo los SWs listos (hay una descarga de RC que tarda aprox 500us)
			ui_timer_set_irq_within_us(MUX_TIME_SWITCHES_us);	//Cargo el tiempo que quiero leer los SW

			//Pongo los pines bajos de los LEDs en alto para que no enciendan independientemente del estado de los SWs
			ui_mux_leds_comm_set_high();				//Pone en SET el estado de los pines bajos de los leds
			ui_mux_io_bus_set_as_input();				//Pone en Hiz los pines de lectura
			
			//Pongo en alto el pin del UI_SW_ENABLE y los pines de lectura en Hiz, ademas configuro la Rpull-down en los pines de lectura
			ui_mux_switches_comm_set_high();

			actual_state = UI_STATE_SWITCHES;
			break;			
	}
}


/* ----------- Codigo Jose ----------- */
uint8_t switches_state_changed (void)
{
	//Funcion que devuelve TRUE si se registro un cambio en el estado de los switches
	if (switches_change_flag == TRUE)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void reset_switches_state_changed (void)
{
	switches_change_flag = FALSE;
}

uint8_t switches_status (void)
{
	//Funcion que analiza el resultado de los switches e informa si hubo cambios respecto al estado anterior
	//Luego de realizar la funcion correspondiete deberia cambiar el estado global de la funcion switches para evitar entrar de nuevo

	static uint8_t switches_states = 0;						//Es el estado actual de los switches, la ultima funcion que se ejecuto

	if(switches_states != ui_mux_get_switches_state())
	{
		//Actualizo el estado de los switches
		switches_states = ui_mux_get_switches_state();
		
		//Hubo un cambio en los switches busco en que switch se efectuo el cambio
		if ( (switches_states & UI_SWITCH_COLDSHOT_SHIFT_MASK) == TRUE )
			//Enciendo el coldshot
			return COLDSHOT_ON_SW;
		else
			//Apago el cold shot
			return COLDSHOT_OFF_SW;
		
		if ( (switches_states & UI_SWITCH_LOCK_SHIFT_MASK) == TRUE )
			//Lock activado
			return LOCK_ON_SW;
		else
			//Lock desactivado
			return LOCK_OFF_SW;

		if ( (switches_states & UI_SWITCH_INC_FAN_SHIFT_MASK) == TRUE )
			//Aumentar velocidad de motor
			return MOTOR_UP_SW;			
		
		if ( (switches_states & UI_SWITCH_DEC_FAN_SHIFT_MASK) == TRUE )
			//Disminuir velocidad de motor
			return MOTOR_DOWN_SW;

		if ( (switches_states & UI_SWITCH_INC_HEAT_SHIFT_MASK) == TRUE )
			//Aumentar velocidad de motor
			return HEAT_UP_SW;
		
		if ( (switches_states & UI_SWITCH_DEC_HEAT_SHIFT_MASK) == TRUE )
			//Disminuir velocidad de motor
			return HEAT_DOWN_SW;
	}
	else
	{
		return NO_CHANGE_SW;			//No hay cambio respecto al estado anterior de los switches
	}
}

/*
void config_gpio_switch_pulldown (void)
{
	//Configura resistencias de pulldown en los pines de entrada
	GPIO_PullResistorConfig(BOARD_UI_IO_1_PORT, BOARD_UI_IO_1_PIN, GPIO_PR_DOWN);
	GPIO_PullResistorConfig(BOARD_UI_IO_2_PORT, BOARD_UI_IO_2_PIN, GPIO_PR_DOWN);
	GPIO_PullResistorConfig(BOARD_UI_IO_3_PORT, BOARD_UI_IO_3_PIN, GPIO_PR_DOWN);
	GPIO_PullResistorConfig(BOARD_UI_IO_4_PORT, BOARD_UI_IO_4_PIN, GPIO_PR_DOWN);
	GPIO_PullResistorConfig(BOARD_UI_IO_5_PORT, BOARD_UI_IO_5_PIN, GPIO_PR_DOWN);
	GPIO_PullResistorConfig(BOARD_UI_IO_6_PORT, BOARD_UI_IO_6_PIN, GPIO_PR_DOWN);
}

void config_gpio_switch_pullup (void)
{
	//Configura resistencias de pullup en los pines de entrada
	GPIO_PullResistorConfig(BOARD_UI_IO_1_PORT, BOARD_UI_IO_1_PIN, GPIO_PR_UP);
	GPIO_PullResistorConfig(BOARD_UI_IO_2_PORT, BOARD_UI_IO_2_PIN, GPIO_PR_UP);
	GPIO_PullResistorConfig(BOARD_UI_IO_3_PORT, BOARD_UI_IO_3_PIN, GPIO_PR_UP);
	GPIO_PullResistorConfig(BOARD_UI_IO_4_PORT, BOARD_UI_IO_4_PIN, GPIO_PR_UP);
	GPIO_PullResistorConfig(BOARD_UI_IO_5_PORT, BOARD_UI_IO_5_PIN, GPIO_PR_UP);
	GPIO_PullResistorConfig(BOARD_UI_IO_6_PORT, BOARD_UI_IO_6_PIN, GPIO_PR_UP);
}


void config_gpio_switch_pull_disable (void)
{
	//Configura resistencias de pullup en los pines de entrada
	GPIO_PullResistorConfig(BOARD_UI_IO_1_PORT, BOARD_UI_IO_1_PIN, GPIO_PR_DISABLE);
	GPIO_PullResistorConfig(BOARD_UI_IO_2_PORT, BOARD_UI_IO_2_PIN, GPIO_PR_DISABLE);
	GPIO_PullResistorConfig(BOARD_UI_IO_3_PORT, BOARD_UI_IO_3_PIN, GPIO_PR_DISABLE);
	GPIO_PullResistorConfig(BOARD_UI_IO_4_PORT, BOARD_UI_IO_4_PIN, GPIO_PR_DISABLE);
	GPIO_PullResistorConfig(BOARD_UI_IO_5_PORT, BOARD_UI_IO_5_PIN, GPIO_PR_DISABLE);
	GPIO_PullResistorConfig(BOARD_UI_IO_6_PORT, BOARD_UI_IO_6_PIN, GPIO_PR_DISABLE);
}
*/