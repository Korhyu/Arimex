#ifndef AL02_SWITCHES_LEDS_MUX_DRIVER_H

#define AL02_SWITCHES_LEDS_MUX_DRIVER_H

#include "AL01_board_iq_aed_rev20191011.h"


#define MUX_TIME_LEDS_uS			4500
#define MUX_TIME_SWITCHES_us	 	500

#define UI_FAN						1
#define UI_HEATER					2

#define UI_UP						1
#define UI_DOWN						2

#define FAN_BIT_MASK				0b00111000
#define HEATER_BIT_MASK				0b00000111

#define UI_SWITCH_COLDSHOT_SHIFT_MASK		5
#define UI_SWITCH_LOCK_SHIFT_MASK			4
#define UI_SWITCH_INC_FAN_SHIFT_MASK		3
#define UI_SWITCH_DEC_FAN_SHIFT_MASK		2
#define UI_SWITCH_INC_HEAT_SHIFT_MASK		1
#define UI_SWITCH_DEC_HEAT_SHIFT_MASK		0

#define UI_LED_MIN_HEAT_SHIFT_MASK			0
#define UI_LED_MID_HEAT_SHIFT_MASK			1
#define UI_LED_MAX_HEAT_SHIFT_MASK			2
#define UI_LED_MIN_FAN_SHIFT_MASK			3
#define UI_LED_MID_FAN_SHIFT_MASK			4
#define UI_LED_MAX_FAN_SHIFT_MASK			5


//Acciones de los switches
#define NO_CHANGE_SW					0              //Sin cambios respecto al estado anterior
#define COLDSHOT_ON_SW 				    1
#define COLDSHOT_OFF_SW	    			2
#define LOCK_ON_SW						3
#define LOCK_OFF_SW			    		4
#define MOTOR_UP_SW				    	5
#define MOTOR_DOWN_SW					6
#define HEAT_UP_SW						7
#define HEAT_DOWN_SW					8


// Puertos --------------------------------
#define UI_LED_ENABLE1a_PORT		GPIO_PB
#define UI_LED_ENABLE1b_PORT		GPIO_PB
#define UI_LED_ENABLE2a_PORT		GPIO_PA
#define UI_LED_ENABLE2b_PORT		GPIO_PB
#define UI_SW_ENABLE_PORT 			GPIO_PB

#define UI_LED_ENABLE1a_HT_PORT		HT_GPIOB
#define UI_LED_ENABLE1b_HT_PORT		HT_GPIOB
#define UI_LED_ENABLE2a_HT_PORT		HT_GPIOA
#define UI_LED_ENABLE2b_HT_PORT		HT_GPIOB
#define UI_SW_ENABLE_HT_PORT 		HT_GPIOB

#define UI_IO_1_PORT		GPIO_PC
#define UI_IO_2_PORT		GPIO_PC
#define UI_IO_3_PORT		GPIO_PA
#define UI_IO_4_PORT		GPIO_PB
#define UI_IO_5_PORT		GPIO_PC
#define UI_IO_6_PORT		GPIO_PA
#define UI_IO_1_HT_PORT		HT_GPIOC
#define UI_IO_2_HT_PORT		HT_GPIOC
#define UI_IO_3_HT_PORT		HT_GPIOA
#define UI_IO_4_HT_PORT		HT_GPIOB
#define UI_IO_5_HT_PORT		HT_GPIOC
#define UI_IO_6_HT_PORT		HT_GPIOA


// Pines ----------------------------------
#define UI_LED_ENABLE1a_PIN			GPIO10
#define UI_LED_ENABLE1b_PIN			GPIO14
#define UI_LED_ENABLE2a_PIN	 		GPIO15
#define UI_LED_ENABLE2b_PIN			GPIO0
#define UI_SW_ENABLE_PIN			GPIO13

#define UI_LED_ENABLE1a_HT_PIN		AFIO_PIN_10
#define UI_LED_ENABLE1b_HT_PIN		AFIO_PIN_14
#define UI_LED_ENABLE2a_HT_PIN	 	AFIO_PIN_15
#define UI_LED_ENABLE2b_HT_PIN		AFIO_PIN_0
#define UI_SW_ENABLE_HT_PIN			AFIO_PIN_13

#define UI_IO_1_PIN			GPIO6
#define UI_IO_2_PIN			GPIO7
#define UI_IO_3_PIN			GPIO8
#define UI_IO_4_PIN			GPIO12
#define UI_IO_5_PIN			GPIO3
#define UI_IO_6_PIN			GPIO6
#define UI_IO_1_HT_PIN		GPIO_PIN_6
#define UI_IO_2_HT_PIN		GPIO_PIN_7
#define UI_IO_3_HT_PIN		GPIO_PIN_8
#define UI_IO_4_HT_PIN		GPIO_PIN_12
#define UI_IO_5_HT_PIN		GPIO_PIN_3
#define UI_IO_6_HT_PIN		GPIO_PIN_6


/* Funciones -------------------------------------------------------------- */
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


//Esta macro configura los pines centrales como SALIDAS - Se deberia llamar en cada cambio de estado de la maquina
#define ui_io_output_state()			{GPIO_DirectionConfig(UI_IO_1_HT_PORT, UI_IO_1_HT_PIN, GPIO_DIR_OUT);\
										 GPIO_DirectionConfig(UI_IO_2_HT_PORT, UI_IO_2_HT_PIN, GPIO_DIR_OUT);\
										 GPIO_DirectionConfig(UI_IO_3_HT_PORT, UI_IO_3_HT_PIN, GPIO_DIR_OUT);\
										 GPIO_DirectionConfig(UI_IO_4_HT_PORT, UI_IO_4_HT_PIN, GPIO_DIR_OUT);\
										 GPIO_DirectionConfig(UI_IO_5_HT_PORT, UI_IO_5_HT_PIN, GPIO_DIR_OUT);\
										 GPIO_DirectionConfig(UI_IO_6_HT_PORT, UI_IO_6_HT_PIN, GPIO_DIR_OUT);\
										 GPIO_PullResistorConfig(UI_IO_1_HT_PORT, UI_IO_1_HT_PIN, GPIO_PR_DISABLE);\
										 GPIO_PullResistorConfig(UI_IO_2_HT_PORT, UI_IO_2_HT_PIN, GPIO_PR_DISABLE);\
										 GPIO_PullResistorConfig(UI_IO_3_HT_PORT, UI_IO_3_HT_PIN, GPIO_PR_DISABLE);\
										 GPIO_PullResistorConfig(UI_IO_4_HT_PORT, UI_IO_4_HT_PIN, GPIO_PR_DISABLE);\
										 GPIO_PullResistorConfig(UI_IO_5_HT_PORT, UI_IO_5_HT_PIN, GPIO_PR_DISABLE);\
										 GPIO_PullResistorConfig(UI_IO_6_HT_PORT, UI_IO_6_HT_PIN, GPIO_PR_DISABLE);}

//Esta macro configura los pines centrales como ENTRADAS - Se deberia llamar en cada cambio de estado de la maquina
#define ui_io_intput_state()			{GPIO_DirectionConfig(UI_IO_1_HT_PORT, UI_IO_1_HT_PIN, GPIO_DIR_IN);\
										 GPIO_DirectionConfig(UI_IO_2_HT_PORT, UI_IO_2_HT_PIN, GPIO_DIR_IN);\
										 GPIO_DirectionConfig(UI_IO_3_HT_PORT, UI_IO_3_HT_PIN, GPIO_DIR_IN);\
										 GPIO_DirectionConfig(UI_IO_4_HT_PORT, UI_IO_4_HT_PIN, GPIO_DIR_IN);\
										 GPIO_DirectionConfig(UI_IO_5_HT_PORT, UI_IO_5_HT_PIN, GPIO_DIR_IN);\
										 GPIO_DirectionConfig(UI_IO_6_HT_PORT, UI_IO_6_HT_PIN, GPIO_DIR_IN);\
										 GPIO_PullResistorConfig(UI_IO_1_HT_PORT, UI_IO_1_HT_PIN, GPIO_PR_DOWN);\
										 GPIO_PullResistorConfig(UI_IO_2_HT_PORT, UI_IO_2_HT_PIN, GPIO_PR_DOWN);\
										 GPIO_PullResistorConfig(UI_IO_3_HT_PORT, UI_IO_3_HT_PIN, GPIO_PR_DOWN);\
										 GPIO_PullResistorConfig(UI_IO_4_HT_PORT, UI_IO_4_HT_PIN, GPIO_PR_DOWN);\
										 GPIO_PullResistorConfig(UI_IO_5_HT_PORT, UI_IO_5_HT_PIN, GPIO_PR_DOWN);\
										 GPIO_PullResistorConfig(UI_IO_6_HT_PORT, UI_IO_6_HT_PIN, GPIO_PR_DOWN);}

#define ui_io_gpio_fun()				{AFIO_GPxConfig(UI_IO_1_HT_PORT, UI_IO_1_HT_PIN, AFIO_FUN_GPIO);\
										 AFIO_GPxConfig(UI_IO_2_HT_PORT, UI_IO_2_HT_PIN, AFIO_FUN_GPIO);\
										 AFIO_GPxConfig(UI_IO_3_HT_PORT, UI_IO_3_HT_PIN, AFIO_FUN_GPIO);\
										 AFIO_GPxConfig(UI_IO_4_HT_PORT, UI_IO_4_HT_PIN, AFIO_FUN_GPIO);\
										 AFIO_GPxConfig(UI_IO_5_HT_PORT, UI_IO_5_HT_PIN, AFIO_FUN_GPIO);\
										 AFIO_GPxConfig(UI_IO_6_HT_PORT, UI_IO_6_HT_PIN, AFIO_FUN_GPIO);}

//Configuro en bajo y alto los pines centrales
#define ui_io_reset_state()				{__hardware_gpio_output_reset(UI_IO_1_PORT, UI_IO_1_PIN);\
										 __hardware_gpio_output_reset(UI_IO_2_PORT, UI_IO_2_PIN);\
										 __hardware_gpio_output_reset(UI_IO_3_PORT, UI_IO_3_PIN);\
										 __hardware_gpio_output_reset(UI_IO_4_PORT, UI_IO_4_PIN);\
										 __hardware_gpio_output_reset(UI_IO_5_PORT, UI_IO_5_PIN);\
										 __hardware_gpio_output_reset(UI_IO_6_PORT, UI_IO_6_PIN);}
#define ui_io_set_state()				{__hardware_gpio_output_set(UI_IO_1_PORT, UI_IO_1_PIN);\
										 __hardware_gpio_output_set(UI_IO_2_PORT, UI_IO_2_PIN);\
										 __hardware_gpio_output_set(UI_IO_3_PORT, UI_IO_3_PIN);\
										 __hardware_gpio_output_set(UI_IO_4_PORT, UI_IO_4_PIN);\
										 __hardware_gpio_output_set(UI_IO_5_PORT, UI_IO_5_PIN);\
										 __hardware_gpio_output_set(UI_IO_6_PORT, UI_IO_6_PIN);}

//Configuro en alto y bajo los pines bajos de los LEDS
#define ui_leds_set_state()				{__hardware_gpio_output_set(BOARD_UI_LED_ENABLE1a_PORT, BOARD_UI_LED_ENABLE1a_PIN);\
										 __hardware_gpio_output_set(BOARD_UI_LED_ENABLE1b_PORT, BOARD_UI_LED_ENABLE1b_PIN);\
										 __hardware_gpio_output_set(BOARD_UI_LED_ENABLE2a_PORT, BOARD_UI_LED_ENABLE2a_PIN);\
										 __hardware_gpio_output_set(BOARD_UI_LED_ENABLE2b_PORT, BOARD_UI_LED_ENABLE2b_PIN);}
#define ui_leds_reset_state()			{__hardware_gpio_output_reset(BOARD_UI_LED_ENABLE1a_PORT, BOARD_UI_LED_ENABLE1a_PIN);\
										 __hardware_gpio_output_reset(BOARD_UI_LED_ENABLE1b_PORT, BOARD_UI_LED_ENABLE1b_PIN);\
										 __hardware_gpio_output_reset(BOARD_UI_LED_ENABLE2a_PORT, BOARD_UI_LED_ENABLE2a_PIN);\
										 __hardware_gpio_output_reset(BOARD_UI_LED_ENABLE2b_PORT, BOARD_UI_LED_ENABLE2b_PIN);}


//Configuro en bajo y alto el pin alto de los SW
#define ui_sw_reset_state()				{__hardware_gpio_output_reset(BOARD_UI_SWITCH_ENABLE_PORT, BOARD_UI_SWITCH_ENABLE_PIN);}
#define ui_sw_set_state()				{__hardware_gpio_output_set(BOARD_UI_SWITCH_ENABLE_PORT, BOARD_UI_SWITCH_ENABLE_PIN);}



uint8_t     ui_mux_get_switches_state   (void);
void 		ui_mux_set_leds_state		(uint8_t led_state);
uint8_t     ui_mux_get_leds_state		(void);
void		ui_mux_leds_state_update 	(void);
void 		ui_mux_init 				(void);

uint8_t     switches_state_changed      (void);
void        reset_switches_state_changed (void);
uint8_t     switches_status             (void);                //Devuelve si se opero alguna accion con la UI
void        config_gpio_switch_pulldown (void);
void        config_gpio_switch_pullup   (void);
void        config_gpio_switch_pull_disable   (void);
uint8_t		read_switches_state			(void);
void		ui_led_change				(int8_t control, int8_t modificador);

#endif /* AL02_SWITCHES_LEDS_MUX_DRIVER_H */
