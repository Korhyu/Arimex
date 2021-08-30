#include "AL02_switches_leds_mux_driver.h"

#define ui_mux_outputs_state_init()		{__hardware_gpio_output_reset(BOARD_UI_LED_ENABLE1a_PORT,BOARD_UI_LED_ENABLE1a_PIN);\
										 __hardware_gpio_output_reset(BOARD_UI_LED_ENABLE1b_PORT,BOARD_UI_LED_ENABLE1b_PIN);\
										 __hardware_gpio_output_reset(BOARD_UI_LED_ENABLE2a_PORT,BOARD_UI_LED_ENABLE2a_PIN);\
										 __hardware_gpio_output_reset(BOARD_UI_LED_ENABLE2b_PORT,BOARD_UI_LED_ENABLE2b_PIN);\
										 __hardware_gpio_output_reset(BOARD_UI_SWITCH_ENABLE_PORT,BOARD_UI_SWITCH_ENABLE_PIN);}

#define ui_mux_leds_comm_enable()		{__hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_LED_ENABLE1a_PORT,BOARD_UI_LED_ENABLE1a_PIN);\
										 __hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_LED_ENABLE1b_PORT,BOARD_UI_LED_ENABLE1b_PIN);\
										 __hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_LED_ENABLE2a_PORT,BOARD_UI_LED_ENABLE2a_PIN);\
										 __hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_LED_ENABLE2b_PORT,BOARD_UI_LED_ENABLE2b_PIN);}

#define ui_mux_leds_comm_disable() 	   {__hardware_gpio_config_set_as_hiz_pin(BOARD_UI_LED_ENABLE1a_PORT,BOARD_UI_LED_ENABLE1a_PIN);\
										 __hardware_gpio_config_set_as_hiz_pin(BOARD_UI_LED_ENABLE1a_PORT,BOARD_UI_LED_ENABLE1a_PIN);\
										 __hardware_gpio_config_set_as_hiz_pin(BOARD_UI_LED_ENABLE1a_PORT,BOARD_UI_LED_ENABLE1a_PIN);\
										 __hardware_gpio_config_set_as_hiz_pin(BOARD_UI_LED_ENABLE1a_PORT,BOARD_UI_LED_ENABLE1a_PIN);}
	
#define ui_mux_leds_comm_set_high()		{__hardware_gpio_output_set(BOARD_UI_LED_ENABLE1a_PORT,BOARD_UI_LED_ENABLE1a_PIN);\
										 __hardware_gpio_output_set(BOARD_UI_LED_ENABLE1b_PORT,BOARD_UI_LED_ENABLE1b_PIN);\
										 __hardware_gpio_output_set(BOARD_UI_LED_ENABLE2a_PORT,BOARD_UI_LED_ENABLE2a_PIN);\
										 __hardware_gpio_output_set(BOARD_UI_LED_ENABLE2b_PORT,BOARD_UI_LED_ENABLE2b_PIN);}

#define ui_mux_leds_comm_set_low()		{__hardware_gpio_output_reset(BOARD_UI_LED_ENABLE1a_PORT,BOARD_UI_LED_ENABLE1a_PIN);\
										 __hardware_gpio_output_reset(BOARD_UI_LED_ENABLE1b_PORT,BOARD_UI_LED_ENABLE1b_PIN);\
										 __hardware_gpio_output_reset(BOARD_UI_LED_ENABLE2a_PORT,BOARD_UI_LED_ENABLE2a_PIN);\
										 __hardware_gpio_output_reset(BOARD_UI_LED_ENABLE2b_PORT,BOARD_UI_LED_ENABLE2b_PIN);}
																				
#define ui_mux_switches_comm_enable()	{__hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_SWITCH_ENABLE_PORT,BOARD_UI_SWITCH_ENABLE_PIN);} 

#define ui_mux_switches_comm_disable() 	{__hardware_gpio_config_set_as_hiz_pin(BOARD_UI_SWITCH_ENABLE_PORT,BOARD_UI_SWITCH_ENABLE_PIN);}

#define ui_mux_io_bus_set_as_output()	{__hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_IO_1_PORT,BOARD_UI_IO_1_PIN);\
										 __hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_IO_2_PORT,BOARD_UI_IO_2_PIN);\
										 __hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_IO_3_PORT,BOARD_UI_IO_3_PIN);\
										 __hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_IO_4_PORT,BOARD_UI_IO_4_PIN);\
										 __hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_IO_5_PORT,BOARD_UI_IO_5_PIN);\
										 __hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_IO_6_PORT,BOARD_UI_IO_6_PIN);}

#define ui_mux_io_bus_set_as_input()	{__hardware_gpio_config_set_as_hiz_pin(BOARD_UI_IO_1_PORT,BOARD_UI_IO_1_PIN);\
										 __hardware_gpio_config_set_as_hiz_pin(BOARD_UI_IO_2_PORT,BOARD_UI_IO_2_PIN);\
										 __hardware_gpio_config_set_as_hiz_pin(BOARD_UI_IO_3_PORT,BOARD_UI_IO_3_PIN);\
										 __hardware_gpio_config_set_as_hiz_pin(BOARD_UI_IO_4_PORT,BOARD_UI_IO_4_PIN);\
										 __hardware_gpio_config_set_as_hiz_pin(BOARD_UI_IO_5_PORT,BOARD_UI_IO_5_PIN);\
										 __hardware_gpio_config_set_as_hiz_pin(BOARD_UI_IO_6_PORT,BOARD_UI_IO_6_PIN);}

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

#define MUX_TIME_LEDS_uS			4500
#define MUX_TIME_SWITCHES_us	 	500

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
void 		ui_mux_init (void)
{
	ui_mux_outputs_state_init();
	board_timer_link_callback(ui_mux_driver_routine,BOARD_TIM_UI_SEL_CALLBACK);
	ui_mux_driver_routine();
}


/*
 * Setea el valor de la variable leds. Para observar el cambio es necesario estar llamando a "ui_mux_leds_state_update"
 */
void 		ui_mux_set_leds_state			(uint8_t led_state)
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
	
	//__hardware_gpio_output_set(GPIOB,GPIO2);			// PIN LIBRE (sin cablear a ningun lado) - Asumo que lo usaba para debug
	
	switch(actual_state)
	{
		case UI_STATE_RESET:		ui_mux_switches_comm_disable();
														ui_mux_leds_state_update();
		
		case UI_STATE_SWITCHES:		ui_timer_set_irq_within_us(MUX_TIME_SWITCHES_us);				
														ui_mux_leds_comm_disable();
														ui_mux_leds_comm_set_high();
														ui_mux_io_bus_set_as_input();
														ui_mux_switches_comm_enable();
														actual_state = UI_STATE_LEDS;		
														break;	
		
		//Estos case no estan invertidos? - En el estado de LEDs se actualizan los switches y viceversa
		case UI_STATE_LEDS:			ui_timer_set_irq_within_us(MUX_TIME_LEDS_uS);
															
														switches = 	(ui_switch_coldshot_get_state()	<<	UI_SWITCH_COLDSHOT_SHIFT_MASK) 	+
																	(ui_switch_lock_get_state()		<<	UI_SWITCH_LOCK_SHIFT_MASK)		+
																	(ui_switch_inc_fan_get_state()	<<	UI_SWITCH_INC_FAN_SHIFT_MASK)	+
																	(ui_switch_dec_fan_get_state()	<<	UI_SWITCH_DEC_FAN_SHIFT_MASK)	+
																	(ui_switch_inc_heat_get_state()	<<	UI_SWITCH_INC_HEAT_SHIFT_MASK) 	+
																	(ui_switch_dec_heat_get_state()	<<	UI_SWITCH_DEC_HEAT_SHIFT_MASK);
		
														//TODO: Quizas aqui previo a actualizar el valor de la variable global deberia consultar si el estado de los switches
														//se mantiene durante 20 o mas veces que se llama a la funcion para que funcione como antirebote
														if (switches == switches_prev)
														{
															//El estado actual es igual al anterior, incremento el contador
															state_count++;
														}
														else
														{
															//El estado anterior era diferente y lo actualizo para la proxima vuelta
															switches_prev = switches;
														}

														if (state_count >= COUNT_DEBOUNCE)
														{
															//Cumpli la cantidad de estados
															state_count = 0;
															switches_change_flag = TRUE;
														}

														switches_state = switches;	//Actualizo variable global de intercambio
		
														ui_mux_switches_comm_disable();
														ui_mux_io_bus_set_as_output();
														ui_mux_leds_comm_set_low();
														ui_mux_leds_comm_enable();
														actual_state = UI_STATE_SWITCHES;
														break;
	}
}


/* ----------- Codigo Jose ----------- */


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