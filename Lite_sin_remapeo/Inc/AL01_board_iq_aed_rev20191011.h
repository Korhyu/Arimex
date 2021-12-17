#ifndef AL01_BOARD_IQ_AED_REV20191011_H

#define AL01_BOARD_IQ_AED_REV20191011_H


#include "AL00_micro_ht32f52241.h"


void board_hardware_configuration (void);

//-------------------------------------------
//				SCHEDULER/TEMPORIZADOR
//-------------------------------------------
int32_t board_scheduler_load_timer			(int32_t time_ms);
int32_t board_scheduler_is_time_expired		(int32_t timer);

//-------------------------------------------
//				      	ADC 
//-------------------------------------------
#define BOARD_ADC_CHANNEL_SHUNT_SEL 						0
#define BOARD_ADC_CHANNEL_VBUS_SEL							1
#define BOARD_ADC_CHANNEL_NTC_PWR_DRIVE_SEL			2
#define BOARD_ADC_CHANNEL_NTC_HEATER_SEL				3



//-------------------------------------------
//				      	BEMF 
//-------------------------------------------
#define U_BEMF_ATT_PIN	GPIO4
#define V_BEMF_ATT_PIN	GPIO5
#define W_BEMF_ATT_PIN	GPIO7	
#define U_BEMF_ATT_PORT GPIOC
#define V_BEMF_ATT_PORT GPIOC
#define W_BEMF_ATT_PORT GPIOA

#define BEMF_VREF_SELECT_PIN	GPIO11
#define BEMF_VREF_SELECT_PORT	GPIOA

#define board_bemf_attenuation_enable()				{__hardware_gpio_config_set_as_pushpull_output_pin(U_BEMF_ATT_PORT,U_BEMF_ATT_PIN);\
																							 __hardware_gpio_config_set_as_pushpull_output_pin(V_BEMF_ATT_PORT,V_BEMF_ATT_PIN);\
																							 __hardware_gpio_config_set_as_pushpull_output_pin(W_BEMF_ATT_PORT,W_BEMF_ATT_PIN);}

#define board_bemf_attenuation_disable()  		{__hardware_gpio_config_set_as_hiz_pin(U_BEMF_ATT_PORT,U_BEMF_ATT_PIN);\
																							 __hardware_gpio_config_set_as_hiz_pin(V_BEMF_ATT_PORT,V_BEMF_ATT_PIN);\
																							 __hardware_gpio_config_set_as_hiz_pin(W_BEMF_ATT_PORT,W_BEMF_ATT_PIN);}

																							 
#define board_bemf_vref_select_vbus()					 __hardware_gpio_output_set  (BEMF_VREF_SELECT_PORT,BEMF_VREF_SELECT_PIN)
#define board_bemf_vref_select_neutral_point() 			__hardware_gpio_output_reset(BEMF_VREF_SELECT_PORT,BEMF_VREF_SELECT_PIN)	 
//-------------------------------------------
//						EXTERNAL COMPS
//-------------------------------------------
#define EXTI_LINE_1											1
#define EXTI_LINE_5											5
#define EXTI_LINE_3											3
#define BOARD_COMP_BEMF_U_PHASE_SELECT	EXTI_LINE_1
#define BOARD_COMP_BEMF_V_PHASE_SELECT	EXTI_LINE_5
#define BOARD_COMP_BEMF_W_PHASE_SELECT	EXTI_LINE_3

#define BOARD_COMP_U_OUTPUT_PIN		GPIO1
#define BOARD_COMP_V_OUTPUT_PIN		GPIO5	
#define BOARD_COMP_W_OUTPUT_PIN		GPIO3
#define BOARD_COMP_U_OUTPUT_PORT	GPIOB
#define BOARD_COMP_V_OUTPUT_PORT	GPIOB
#define BOARD_COMP_W_OUTPUT_PORT	GPIOB

#define BOARD_COMP_BEMF_IS_ABOVE_VREF								SET
#define BOARD_COMP_BEMF_IS_BELOW_VREF								RESET

#define board_comp_i_peak_link_callback(func_ptr)		hardware_pwm_break_function_link_callback(func_ptr)
#define board_comp_i_peak_disable_irq()						__hardware_pwm_break_disable_irq()
#define board_comp_i_peak_enable_irq()						__hardware_pwm_break_enable_irq()


#define board_comp_bemf_rising_enable_irq(phase_select)								__hardware_gpio_event_rising_only_detection_enable(phase_select)
#define board_comp_bemf_falling_enable_irq(phase_select)							__hardware_gpio_event_falling_only_detection_enable(phase_select)
#define board_comp_bemf_disable_irq(phase_select)										__hardware_gpio_event_detection_disable(phase_select)

#define board_comp_bemf_disable_irqs_all_phases()											{board_comp_bemf_disable_irq(BOARD_COMP_BEMF_U_PHASE_SELECT);\
																																			 board_comp_bemf_disable_irq(BOARD_COMP_BEMF_V_PHASE_SELECT);\
																																			 board_comp_bemf_disable_irq(BOARD_COMP_BEMF_W_PHASE_SELECT);}

#define board_comp_bemf_enable_rising_irqs_all_phases()								{board_comp_bemf_rising_enable_irq(BOARD_COMP_BEMF_U_PHASE_SELECT);\
																																			 board_comp_bemf_rising_enable_irq(BOARD_COMP_BEMF_V_PHASE_SELECT);\
																																			 board_comp_bemf_rising_enable_irq(BOARD_COMP_BEMF_W_PHASE_SELECT);}

#define board_comp_bemf_rising_link_callback(func_ptr,phase_select)		hardware_gpio_event_rising_event_link_callback(func_ptr,phase_select)
#define board_comp_bemf_falling_link_callback(func_ptr,phase_select)	hardware_gpio_event_falling_event_link_callback(func_ptr,phase_select)

#define board_comp_bemf_get_output_u_phase()	__hardware_gpio_input_read_state(BOARD_COMP_U_OUTPUT_PORT,BOARD_COMP_U_OUTPUT_PIN)
#define board_comp_bemf_get_output_v_phase()	__hardware_gpio_input_read_state(BOARD_COMP_V_OUTPUT_PORT,BOARD_COMP_V_OUTPUT_PIN)
#define board_comp_bemf_get_output_w_phase()	__hardware_gpio_input_read_state(BOARD_COMP_W_OUTPUT_PORT,BOARD_COMP_W_OUTPUT_PIN)
																																			 
//-------------------------------------------
//			GPIO - OUTPUS PP MISC
//-------------------------------------------
//	Defines de pines de GPIO outputs! para las funciones de config
#define board_pin_set(port, pin)												__hardware_gpio_output_set(port, pin)
#define board_pin_reset(port, pin)											__hardware_gpio_output_reset(port, pin)
#define board_pin_toggle(port, pin)											__hardware_gpio_output_toggle(gpio_port,GPIOnum)								


//-------------------------------------------
//				 PWM PRINCIPAL
//-------------------------------------------
// Defines para saber que pines del PWM hay.. hay que configurar los pine de LIx
#define BOARD_LIN1_PIN	GPIO0
#define BOARD_LIN2_PIN	GPIO2
#define BOARD_LIN3_PIN	GPIO6
#define BOARD_LIN1_PORT GPIOA
#define BOARD_LIN2_PORT GPIOC
#define BOARD_LIN3_PORT GPIOB

#define board_pwm_set_sense_on_toff_before_ton_us(time_before_ton_us)	hardware_pwm_set_sense_on_toff_before_ton_us(time_before_ton_us)

#define board_pwm_break_disable_irq()									__hardware_pwm_break_disable_irq()
#define board_pwm_break_enable_irq()									__hardware_pwm_break_enable_irq()
#define board_pwm_end_toff_link_callback(func_ptr)						hardware_pwm_end_toff_link_callback(func_ptr)

#define board_pwm_set_period_us(period_us)								hardware_pwm_set_period_us(period_us)
#define board_pwm_get_period_us()										hardware_pwm_get_period_us()
#define	board_pwm_set_ton_us(ton_us)									hardware_pwm_set_ton_us(ton_us)
#define board_pwm_get_ton_us()											hardware_pwm_get_ton_us()
#define board_pwm_set_toff_us(toff_us)								 	hardware_pwm_set_toff_us(toff_us)
#define	board_pwm_get_toff_us()											harwdare_pwm_get_toff_us()
#define	board_pwm_set_counter_to_toff()									hardware_pwm_set_counter_to_toff()								//Jose
#define	board_pwm_reset_counter()										hardware_pwm_reset_counter()									//Jose


#define board_pwm_hin1_enable()												__hardware_pwm_hin1_enable()
#define board_pwm_hin2_enable()												__hardware_pwm_hin2_enable()
#define board_pwm_hin3_enable()												__hardware_pwm_hin3_enable()
#define board_pwm_lin1_enable()												__hardware_gpio_output_set(BOARD_LIN1_PORT,BOARD_LIN1_PIN)	
#define board_pwm_lin2_enable()												__hardware_gpio_output_set(BOARD_LIN2_PORT,BOARD_LIN2_PIN)	
#define board_pwm_lin3_enable()												__hardware_gpio_output_set(BOARD_LIN3_PORT,BOARD_LIN3_PIN)	

#define board_pwm_hin1_disable()											__hardware_pwm_hin1_disable()
#define board_pwm_hin2_disable()											__hardware_pwm_hin2_disable()
#define board_pwm_hin3_disable()											__hardware_pwm_hin3_disable()
#define board_pwm_lin1_disable()											__hardware_gpio_output_reset(BOARD_LIN1_PORT,BOARD_LIN1_PIN)	
#define board_pwm_lin2_disable()											__hardware_gpio_output_reset(BOARD_LIN2_PORT,BOARD_LIN2_PIN)	
#define board_pwm_lin3_disable()											__hardware_gpio_output_reset(BOARD_LIN3_PORT,BOARD_LIN3_PIN)	

#define board_pwm_start_with_ton()											hardware_pwm_start_with_ton()
#define board_pwm_set_outputs_to_toff()							    		hardware_pwm_set_outputs_to_toff()

#define board_pwm_start_with_toff()							    			hardware_pwm_set_outputs_to_toff()

void hardware_pwm_break_function_link_callback (void (*func_pointer)(void));
																																			 
																																			 
//-------------------------------------------
//				 GPIO EVENTS
//-------------------------------------------
// PInes tipo GPIOevents, aca tambien hay que poner COMPARADORES


//-------------------------------------------
//				 TIMERS
//-------------------------------------------
#define BOARD_TIM_WATCHDOG_SEL_CALLBACK		__HARDWARE_TIM_BFTM0_SEL
#define BOARD_TIM_COMM_SEL_CALLBACK			__HARDWARE_TIM_BFTM1_SEL
#define BOARD_TIM_BLANKING_SEL_CALLBACK		__HARDWARE_TIM_SCTM0_SEL
#define BOARD_TIM_UI_SEL_CALLBACK			__HARDWARE_TIM_SCTM1_SEL

#define BOARD_TIM_BFTM_WATCHDOG				HT_BFTM0
#define BOARD_TIM_BFTM_COMM					HT_BFTM1
#define	BOARD_TIM_SCTM_BLANKING				HT_SCTM0
#define BOARD_TIM_SCTM_UI					HT_SCTM1
																																		 
																																							 
#define board_timer_link_callback(func_ptr,__hardware_tim_sel)	hardware_htim_link_callback(func_ptr,__hardware_tim_sel)

#define board_tim_bftm_init_timer_with_timeout_irq_us(BOARD_TIM_BFTM_X,time_1us_cnt)	__harwdare_tim_bftm_init_timer_with_timeout_irq_us(BOARD_TIM_BFTM_X,time_1us_cnt)
#define board_tim_sctm_init_timer_with_timeout_irq_us(BOARD_TIM_SCTM_X,time_1us_cnt)	__hardware_tim_sctm_init_timer_with_timeout_irq_us(BOARD_TIM_SCTM_X,time_1us_cnt)

#define board_tim_bftm_abort(BOARD_TIM_BFTM_X)	__harwdare_tim_bftm_abort(BOARD_TIM_BFTM_X)			
#define board_tim_sctm_abort(BOARD_TIM_SCTM_X)	__harwdare_tim_sctm_abort(BOARD_TIM_SCTM_X)		

#define board_tim_get_bftm_count_us(BOARD_TIM_BFTM_X)		__hardware_tim_bftm_get_count_us(BOARD_TIM_BFTM_X)
#define board_tim_get_sctm_count_us(BOARD_TIM_SCTM_X)		__hardware_tim_sctm_get_count_us(BOARD_TIM_SCTM_X)
																																																			
//-------------------------------------------
//				      	ADC 
//-------------------------------------------
#define board_adc_get_rank0()	(HT_ADC->DR[0] & 0x0FFF)
#define board_adc_get_rank1()	(HT_ADC->DR[1] & 0x0FFF)
#define board_adc_get_rank2()	(HT_ADC->DR[2] & 0x0FFF)
#define board_adc_get_rank3()	(HT_ADC->DR[3] & 0x0FFF)

int32_t board_adc_get_vbus_voltage_C (void);
int32_t board_adc_get_shunt_average_current_mA (void);
int32_t board_adc_get_power_drivers_temp_C (void);
int32_t board_adc_get_heater_temp_C (void);


//-------------------------------------------
//				      	GPIOS 
//-------------------------------------------
#define BOARD_UI_IO_1_PIN						GPIO6
#define BOARD_UI_IO_1_PORT						GPIOC
#define BOARD_UI_IO_2_PIN						GPIO7
#define BOARD_UI_IO_2_PORT						GPIOC
#define BOARD_UI_IO_3_PIN						GPIO8
#define BOARD_UI_IO_3_PORT						GPIOA
#define BOARD_UI_IO_4_PIN						GPIO12
#define BOARD_UI_IO_4_PORT						GPIOB
#define BOARD_UI_IO_5_PIN						GPIO3
#define BOARD_UI_IO_5_PORT						GPIOC
#define BOARD_UI_IO_6_PIN						GPIO6
#define BOARD_UI_IO_6_PORT						GPIOA
#define BOARD_UI_WAKE_PIN						GPIO0	
#define BOARD_UI_WAKE_PORT						GPIOC
#define BOARD_UI_LED_ENABLE1a_PIN				GPIO10
#define BOARD_UI_LED_ENABLE1a_PORT				GPIOB
#define BOARD_UI_LED_ENABLE1b_PIN				GPIO14
#define BOARD_UI_LED_ENABLE1b_PORT				GPIOB
#define BOARD_UI_LED_ENABLE2a_PIN				GPIO15
#define BOARD_UI_LED_ENABLE2a_PORT				GPIOA
#define BOARD_UI_LED_ENABLE2b_PIN				GPIO0
#define BOARD_UI_LED_ENABLE2b_PORT				GPIOB
#define	BOARD_UI_SWITCH_ENABLE_PIN				GPIO13
#define BOARD_UI_SWITCH_ENABLE_PORT				GPIOB
#define	BOARD_HEATER_GATE1_PIN					GPIO3
#define BOARD_HEATER_GATE1_PORT					GPIOA
#define	BOARD_HEATER_GATE2_PIN					GPIO11
#define BOARD_HEATER_GATE2_PORT					GPIOB
#define	BOARD_HEATER_ZCD_DET_PIN				GPIO14
#define BOARD_HEATER_ZCD_DET_PORT				GPIOA

#define GPIOA_PIN_6_HIZ							(1<<GPIO6)
#define GPIOA_PIN_15_HIZ						(1<<GPIO15)
#define GPIOB_PIN_10_HIZ						(1<<GPIO10)
#define GPIOB_PIN_14_HIZ						(1<<GPIO14)
#define GPIOB_PIN_0_HIZ							(1<<GPIO0)
#define GPIOB_PIN_13_HIZ						(1<<GPIO13)
#define GPIOC_PIN_3_HIZ							(1<<GPIO3)
#define GPIOC_PIN_0_HIZ							(1<<GPIO0)


#define GPIOC_PIN_6_IN_PULLUP					(1<<GPIO6)
#define GPIOC_PIN_7_IN_PULLUP					(1<<GPIO7)
#define GPIOA_PIN_8_IN_PULLUP					(1<<GPIO8)
#define GPIOB_PIN_12_IN_PULLUP					(1<<GPIO12)

#define board_hardware_gpio_config_output_pp_pins_load_config(gpio_port,gpio_pin)		hardware_gpio_config_output_pp_pins_load_config(gpio_port,gpio_pin)
#define board_hardware_gpio_config_edge_events_pins_load_config(gpio_port,gpio_pin)		hardware_gpio_config_edge_events_pins_load_config(gpio_port,gpio_pin)
#define board_hardware_gpio_config_hiz_pins_load_config(gpio_port,gpio_pin)				hardware_gpio_config_hiz_pins_load_config(gpio_port,gpio_pin)

#define	board_gpio_event_rising_link_callback(func_ptr,exti_line)						hardware_gpio_event_rising_event_link_callback(func_ptr,exti_line)
#define	board_gpio_event_falling_link_callback(func_ptr,exti_line)						hardware_gpio_event_falling_event_link_callback(func_ptr,exti_line)

#define board_hardware_gpio_config_inputs_pins_load_config(gpio_port,gpio_pin)			hardware_gpio_config_inputs_pins_load_config(gpio_port,gpio_pin)


#define	board_gpio_event_rising_only_detection_enable(gpio_pin) 	  		  			__hardware_gpio_event_rising_only_detection_enable(gpio_pin)
#define	board_gpio_event_falling_only_detection_enable(gpio_pin)	  		  			__hardware_gpio_event_falling_only_detection_enable(gpio_pin)
#define	board_gpio_event_both_edges_detection_enable(gpio_pin)	  	  	 	  			__hardware_gpio_event_both_edges_detection_enable(gpio_pin)
#define board_gpio_event_detection_disable(gpio_pin)				  		  			__hardware_gpio_event_detection_disable(gpio_pin)




//-------------------------------------------
//			GPIO - HIZ - "OPEN DRAINS"
//-------------------------------------------
#define board_gpio_hiz_set_as_hiz(gpio_port,gpio_pin)					__hardware_gpio_config_set_as_hiz_pin(gpio_port,gpio_pin)
#define board_gpio_hiz_set_as_pushpull_output_pin(gpio_port,gpio_pin)	__hardware_gpio_config_set_as_pushpull_output_pin(gpio_port,gpio_pin)


//-------------------------------------------
//			GPIO - HIZ - "INPUTS"
//-------------------------------------------
#define board_gpio_input_read_state(gpio_port,gpio_pin) 				__hardware_gpio_input_read_state(gpio_port,gpio_pin)


//-------------------------------------------
//						UART
//-------------------------------------------
#define board_uart_read()												hardware_uart_read()
#define board_uart_write												hardware_uart_transmit



#endif /* AL01_BOARD_IQ_AED_REV20191011_H */
