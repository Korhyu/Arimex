#include "stm32f0xx_hal.h"
#include "stdint.h"

void hardware_config (void);

void SystemClock_Config(void);

uint8_t hardware_uart_read (void);


volatile uint32_t * hardware_adc_get_data_p			    (void);
int32_t						  hardware_adc_get_channels_count  (void);

void hardware_tim15_watchdog_callback(void);
void hardware_tim_blanking_callback(void);
void hardware_comp_i_peak_callback(void);
void hardware_comp_bemf_callback(void);
void hardware_tim3_comm_callback(void);

void exti0_rising_edge_callback(void);
void exti0_falling_edge_callback(void);


extern TIM_HandleTypeDef htim1,htim2,htim3,htim15;
extern UART_HandleTypeDef huart1;
extern ADC_HandleTypeDef hadc;
extern DMA_HandleTypeDef hdma_adc;
extern COMP_HandleTypeDef hcomp1,hcomp2;

extern int32_t pwm_time_before_ton_start_us;

#define ADC_CHANNEL_COUNT 4


//------------------------------------------------
//Macros de operacion de perisferico GPIO OUTPUTS
//------------------------------------------------
	//GPIO_OUPUTS_PP		  : se usan para controlar señales relacionadas al sensado 	 - LIN1 : PA7
	//											de BEMF, para multiplexar PWM (LIN1,LIN2,LIN3) y para		 - LIN2 : PB0
	//											controlar leds.																					 - LIN3 : PB2
	//																																							 - LED1 : PB14
	//																																							 - LED2 : PB15
	//																																							 - LED3 : PB13
	//																																							 - LED4 : PB12
	//																																							 - LED5 : PD2
	//																																							 - LED6 : PB4
	//																																							 - LED7 : PB5
	//																																							 - LED8 : PF4 (OVERTEMP)
	//																																							 - LED9 : PA15(SYNC_FAIL)
	//																																							 - BLANKING_MODE: PC8
	//																																							 - BEMF_SLOPE : PC9
	//																																							 - BEMF_REF_SELECT: PC10


#define __hardware_bemf_slope_to_sense_positive()								HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET)
#define __hardware_bemf_slope_to_sense_negative()								HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET)
#define __hardware_bemf_vref_select_vbus()											HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_SET)
#define __hardware_bemf_vref_select_virtual_neutral_point()			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_RESET)

#define __hardware_led_1_set()	 	  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_SET)   //Set LED1 
#define __hardware_led_2_set()	 	  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_SET)   //Set LED2 
#define __hardware_led_3_set()	 	  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_SET)   //Set LED3 
#define __hardware_led_4_set()	 	  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET)   //Set LED4
#define __hardware_led_5_set()	 	  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET)    //Set LED5
#define __hardware_led_6_set()	 	  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET)    //Set LED6 
#define __hardware_led_7_set()	 	  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET)    //Set LED7 
#define __hardware_led_8_set()	 	  HAL_GPIO_WritePin(GPIOF,GPIO_PIN_4,GPIO_PIN_SET)    //Set LED8
#define __hardware_led_9_set()	 	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_SET)   //Set LED9

#define __hardware_led_1_reset()	  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_RESET)   //Reset LED1 
#define __hardware_led_2_reset()	  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_RESET)   //Reset LED2 
#define __hardware_led_3_reset()	  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_RESET)   //Reset LED3 
#define __hardware_led_4_reset()	  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET)   //Reset LED4
#define __hardware_led_5_reset()	  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET)    //Reset LED5
#define __hardware_led_6_reset()	  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET)    //Reset LED6 
#define __hardware_led_7_reset()	  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET)    //Reset LED7 
#define __hardware_led_8_reset()	  HAL_GPIO_WritePin(GPIOF,GPIO_PIN_4,GPIO_PIN_RESET)    //Reset LED8
#define __hardware_led_9_reset()	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_RESET)   //Reset LED9

#define __hardware_led_1_toggle() 	HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_14)
#define __hardware_led_2_toggle()	 	HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_15)
#define __hardware_led_3_toggle() 	HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_13)
#define __hardware_led_4_toggle() 	HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_12)
#define __hardware_led_5_toggle() 	HAL_GPIO_TogglePin(GPIOD,GPIO_PIN_2)
#define __hardware_led_6_toggle() 	HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_4)
#define __hardware_led_7_toggle() 	HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_5)
#define __hardware_led_8_toggle() 	HAL_GPIO_TogglePin(GPIOF,GPIO_PIN_4)
#define __hardware_led_9_toggle() 	HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_15)

#define __hardware_led_bar_max_set() 		__hardware_led_5_set()
#define __hardware_led_bar_max_reset()	__hardware_led_5_reset()
#define __hardware_led_bar_max_toggle()	__hardware_led_5_toggle()

#define __hardware_led_bar_med_set()		__hardware_led_6_set()
#define __hardware_led_bar_med_reset()	__hardware_led_6_reset()
#define __hardware_led_bar_med_toggle()	__hardware_led_6_toggle()

#define __hardware_led_bar_min_set()		__hardware_led_7_set()
#define __hardware_led_bar_min_reset()	__hardware_led_7_reset()
#define __hardware_led_bar_min_toggle()	__hardware_led_7_toggle()


#define __hardware_led_overtemp_set() 		__hardware_led_8_set()
#define __hardware_led_overtemp_reset()		__hardware_led_8_reset()
#define __hardware_led_overtemp_toggle()  __hardware_led_8_toggle()
#define __hardware_led_fail_sync_set()		__hardware_led_9_set()
#define __hardware_led_fail_sync_reset()	__hardware_led_9_reset()
#define __hardware_led_fail_sync_toggle() __hardware_led_9_toggle()


//--------------------------------------------------
//Macros de operacion de perisferico Digital Inputs
//--------------------------------------------------

	//GPIO_INPUTS					: se usan para leer pulsadores 														 - PSW0 : PC11 (SLIDE SWITCH)
	//																																							 - PSW1 : PB9
	//																																							 - PSW2 : PC13
	//																																							 - PSW3 : PF7
	//																																							 - PSW4 : PF6
	//																																							 - PSW5 : PB8
	//																																							 - PSW6 : PC12

#define __hardware_tactswitch_0_get_state()						HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_11)
#define __hardware_tactswitch_1_get_state()						HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9)
#define __hardware_tactswitch_2_get_state()						HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13)
#define __hardware_tactswitch_3_get_state()						HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_7)
#define __hardware_tactswitch_4_get_state()						HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_6)
#define __hardware_tactswitch_5_get_state()						HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_8)
#define __hardware_tactswitch_6_get_state()						HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_12)



//------------------------------------------------
//Macros de operacion de perisferico outputs-hiZ
//------------------------------------------------
	//GPIOs HIz/OD - Multiplexacion de las señales de PWM. HIN1 HIN2 y HIN3
	//						 - HIN1 : ATENCION: En REV2019-00 no esta cableado correctamente. En REV2019-01 es PC6
	//						 - HIN2 : PA9
	//						 - HIN3 : PA10
	
#define GLOBAL_SET_PIN_AS_OUTPUT(GPIOx,PIN_NUM) 			{GPIOx->MODER=GPIOx->MODER&(~(0x03<<((PIN_NUM)<<1)));GPIOx->MODER=GPIOx->MODER|(1<<((PIN_NUM)<<1));};
#define GLOBAL_SET_PIN_AS_HIZ(GPIOx,PIN_NUM)					(GPIOx->MODER=(GPIOx->MODER&~(3<<(PIN_NUM<<1))))
#define GLOBAL_SET_PIN_AS_ANALOG_INPUT(GPIOx,PIN_NUM)	{(GPIOx->MODER=(GPIOx->MODER&~(3<<(PIN_NUM<<1))));GPIOx->MODER=GPIOx->MODER|(3<<((PIN_NUM)<<1));}



//-------------------------------------------------------------
//Macros de operacion de perisferico PWM
//-------------------------------------------------------------

//Cuando salte el comparador por sobrecorriente a su ISR, va a hacer un set_counter(autoreload_counter) 
//Seteando el contador al final de ton y contará hasta el final y comenzará el ciclo de PWM denuevo conduciendo 
//full hasta el proximo limite de corriente, y asi.

void hardware_pwm_set_period_us(uint32_t period_us);

void hardware_pwm_set_ton_us(uint32_t ton_us);

void hardware_pwm_set_toff_us(uint32_t toff_us);

void hardware_pwm_set_outputs_to_toff(void);

uint32_t hardware_pwm_get_ton_elapsed(void);

void hardware_pwm_reset_counter(void);

void hardware_pwm_start_from_0(void);

void hardware_pwm_set_time_for_sense_zcd_before_ton_starts_us(uint32_t time_us);

uint32_t hardware_pwm_get_ton(void);

#define __hardware_pwm_set_period_us(period_us)	{\
				__HAL_TIM_SET_AUTORELOAD(&htim1,(period_us-1));\
				TIM1->CR1=TIM1->CR1&(~(1<<3));\
				TIM1->EGR|=0x0001;\
				}
	
#define __hardware_pwm_set_toff_us(toff_us) {\
			__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,((toff_us<pwm_time_before_ton_start_us) ? pwm_time_before_ton_start_us : toff_us));\
			__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,((toff_us<pwm_time_before_ton_start_us) ? 1 : toff_us-pwm_time_before_ton_start_us+1));\
				TIM1->CR1=TIM1->CR1&(~(1<<3));\
				TIM1->EGR|=0x0001;\
				}

#define __hardware_pwm_set_ton_us(ton_us) {\
				__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,((ton_us+pwm_time_before_ton_start_us)>=__HAL_TIM_GET_AUTORELOAD(&htim1))?(__HAL_TIM_GET_AUTORELOAD(&htim1)-pwm_time_before_ton_start_us+2):(__HAL_TIM_GET_AUTORELOAD(&htim1)-ton_us+1));\
				__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,((ton_us+pwm_time_before_ton_start_us)>=__HAL_TIM_GET_AUTORELOAD(&htim1))?1:(__HAL_TIM_GET_AUTORELOAD(&htim1)-ton_us-pwm_time_before_ton_start_us+1));\
				TIM1->CR1=TIM1->CR1&(~(1<<3));\
				TIM1->EGR|=0x0001;\
				}


//Cuando salta el limite de corriente se llama a esta funcion para que ponga las salidas de PWM en 0 				
#define __hardware_pwm_set_outputs_to_toff() {\
																							TIM1->EGR|=0x0001;\
																							TIM1->CR1=TIM1->CR1&(~(1<<3));\
																							__HAL_TIM_SET_COUNTER(&htim1,1);\
																						 }	
				
				
#define __hardware_pwm_get_ton_elapsed()		 __HAL_TIM_GET_COUNTER(&htim1)
																						 
#define __hardware_pwm_set_zero_duty() 			 __hardware_pwm_set_toff_us(0xFFFF)
																						 
#define __hardware_pwm_stop_counter()				 __HAL_TIM_DISABLE(&htim1);
#define __hardware_pwm_resume_counter()			 __HAL_TIM_ENABLE(&htim1);
#define __hardware_pwm_reset_counter()			 {__HAL_TIM_SET_COUNTER(&htim1,0);TIM1->CR1=TIM1->CR1&(~(1<<3));TIM1->EGR|=0x0001;}
#define __hardware_pwm_start_from_0()				 {__HAL_TIM_SET_COUNTER(&htim1,__HAL_TIM_GET_COMPARE(&htim1,TIM_CHANNEL_1));__HAL_TIM_ENABLE(&htim1);}
	
																							 
#define __hardware_pwm_hin1_set()		{GLOBAL_SET_PIN_AS_HIZ(GPIOC,6);GLOBAL_SET_PIN_AS_HIZ(GPIOB,9);}/*SACAR LA INSTRUCCION DE PB9 EN VERSIONES DE HW SUPERIORES A LA 00*/
#define __hardware_pwm_hin2_set()		 GLOBAL_SET_PIN_AS_HIZ(GPIOA,9)
#define __hardware_pwm_hin3_set()		 GLOBAL_SET_PIN_AS_HIZ(GPIOA,10)
#define __hardware_pwm_hin1_reset()	{GLOBAL_SET_PIN_AS_OUTPUT(GPIOC,6);GLOBAL_SET_PIN_AS_OUTPUT(GPIOB,9);} /*SACAR LA INSTRUCCION DE PB9 EN VERSIONES DE HW SUPERIORES A LA 00*/
#define __hardware_pwm_hin2_reset()	 GLOBAL_SET_PIN_AS_OUTPUT(GPIOA,9)
#define __hardware_pwm_hin3_reset()	 GLOBAL_SET_PIN_AS_OUTPUT(GPIOA,10)


#define __hardware_bemf_blanking_disable_all()								GLOBAL_SET_PIN_AS_OUTPUT(GPIOF,6)					
#define __hardware_bemf_blanking_not_disable_all()						GLOBAL_SET_PIN_AS_HIZ(GPIOF,6)		

#define __hardware_bemf_blanking_mode_blank_toff_disable()			GLOBAL_SET_PIN_AS_OUTPUT(GPIOC,13)
#define __hardware_bemf_blanking_mode_blank_ton_disable()				GLOBAL_SET_PIN_AS_OUTPUT(GPIOF,7)
#define __hardware_bemf_blanking_mode_blank_toff_enable()				GLOBAL_SET_PIN_AS_HIZ(GPIOC,13)
#define __hardware_bemf_blanking_mode_blank_ton_enable()				GLOBAL_SET_PIN_AS_HIZ(GPIOF,7)

#define __hardware_bemf_blanking_mode_set_blanking_on_ton_only()  __hardware_bemf_blanking_disable_all()
#define __hardware_bemf_blanking_mode_set_blanking_on_toff_only()  __hardware_bemf_blanking_disable_all()
#define __hardware_bemf_blanking_mode_set_blanking_only_on_edges() __hardware_bemf_blanking_disable_all()
#define __hardware_bemf_blanking_mode_set_blanking_disable()			 __hardware_bemf_blanking_disable_all()



#define __hardware_pwm_lin1_set()		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_SET)
#define __hardware_pwm_lin2_set()		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET)
#define __hardware_pwm_lin3_set()		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,GPIO_PIN_SET)
#define __hardware_pwm_lin1_reset()	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET)	
#define __hardware_pwm_lin2_reset()	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET)
#define __hardware_pwm_lin3_reset()	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,GPIO_PIN_RESET)					


#define __HARDWARE_PWM_TON_CORRECTION_uS 1	/*Esto es debido a que el circuito de Timming modifica levemente la duración de Ton */
#define __HARDWARE_PWM_TON_MIN_uS				 8 	/*Debido al circuito del timming, el tiempo de encendido minimo es de 7uS*/


#define __SET_COMP2_INV_INPUT_PA4() COMP->CSR=(COMP->CSR&~COMP_CSR_COMP2INSEL_Msk)|(4<<20)
#define __SET_COMP2_INV_INPUT_PA5() COMP->CSR=(COMP->CSR&~COMP_CSR_COMP2INSEL_Msk)|(5<<20)
#define __SET_COMP2_INV_INPUT_PA2() COMP->CSR=(COMP->CSR&~COMP_CSR_COMP2INSEL_Msk)|(6<<20)
#define __SET_COMP1_INV_INPUT_PA4() COMP->CSR=(COMP->CSR&~COMP_CSR_COMP1INSEL_Msk)|(4<<4)
#define __SET_COMP1_INV_INPUT_PA5() COMP->CSR=(COMP->CSR&~COMP_CSR_COMP1INSEL_Msk)|(5<<4)
#define __SET_COMP1_INV_INPUT_PA0() COMP->CSR=(COMP->CSR&~COMP_CSR_COMP1INSEL_Msk)|(6<<4)

#define __hardware_comp_bemf_sense_u_phase()	__SET_COMP1_INV_INPUT_PA0()
#define __hardware_comp_bemf_sense_v_phase() 	__SET_COMP1_INV_INPUT_PA4()
#define __hardware_comp_bemf_sense_w_phase() 	__SET_COMP1_INV_INPUT_PA5()

#define __SET_COMP2_DISABLE() COMP->CSR&=(~(1<<16))
#define __SET_COMP2_ENABLE()  {COMP->CSR|=(1<<16);}
#define __SET_COMP1_DISABLE() COMP->CSR&=(~(1<<0))
#define __SET_COMP1_ENABLE()  {COMP->CSR|=(1<<0);}

#define __hardware_comp_bemf_clear_flags()							__HAL_COMP_COMP1_EXTI_CLEAR_FLAG()


#define __hardware_comp_bemf_enable_sense() 						__SET_COMP1_ENABLE()
#define __hardware_comp_bemf_disable_sense() 						__SET_COMP1_DISABLE()
#define __hardware_comp_bemf_enable_irq()								(EXTI->IMR|=( COMP_EXTI_LINE_COMP1))
#define __hardware_comp_bemf_disable_irq()							(EXTI->IMR&=(~COMP_EXTI_LINE_COMP1))

#define __hardware_comp_i_peak_enable_sense()						__SET_COMP2_ENABLE()
#define __hardware_comp_i_peak_disable_sense()					__SET_COMP2_DISABLE()


#define __hardware_comp1_set_falling_edge_irq() 	 			 (EXTI->FTSR|=( COMP_EXTI_LINE_COMP1))
#define __hardware_comp1_reset_falling_edge_irq() 			 (EXTI->FTSR&=(~COMP_EXTI_LINE_COMP1))
#define __hardware_comp1_set_rising_edge_irq()		 			 (EXTI->RTSR|=( COMP_EXTI_LINE_COMP1))
#define __hardware_comp1_reset_rising_edge_irq()  	 		 (EXTI->RTSR&=(~COMP_EXTI_LINE_COMP1))
#define __hardware_comp1_set_rising_edge_irq_only()			 {__hardware_comp1_set_rising_edge_irq();__hardware_comp1_reset_falling_edge_irq();}
#define __hardware_comp1_set_falling_edge_irq_only()		 {__hardware_comp1_reset_rising_edge_irq();__hardware_comp1_set_falling_edge_irq();}
#define __hardware_comp1_set_both_edges_irq()						 {__hardware_comp1_set_falling_edge_irq();__hardware_comp1_set_rising_edge_irq();}

#define __hardware_comp_bemf_set_falling_edge_irq() 	 		__hardware_comp1_set_falling_edge_irq()
#define __hardware_comp_bemf_reset_falling_edge_irq() 		__hardware_comp1_reset_falling_edge_irq()
#define __hardware_comp_bemf_set_rising_edge_irq()		 		__hardware_comp1_set_rising_edge_irq()
#define __hardware_comp_bemf_reset_rising_edge_irq()  	 	__hardware_comp1_reset_rising_edge_irq()
#define __hardware_comp_bemf_set_rising_edge_irq_only()		__hardware_comp1_set_rising_edge_irq_only()
#define __hardware_comp_bemf_set_falling_edge_irq_only()	__hardware_comp1_set_falling_edge_irq_only()
#define __hardware_comp_bemf_set_both_edges_irq()					__hardware_comp1_set_both_edges_irq()
#define __hardware_comp_bemf_reset_both_edges_irq()				{__hardware_comp_bemf_reset_falling_edge_irq();__hardware_comp_bemf_reset_rising_edge_irq();}

#define __hardware_comp2_set_falling_edge_irq() 	 					EXTI->FTSR|=( COMP_EXTI_LINE_COMP2)
#define __hardware_comp2_reset_falling_edge_irq() 					EXTI->FTSR&=(~COMP_EXTI_LINE_COMP2)
#define __hardware_comp2_set_rising_edge_irq()		 					EXTI->RTSR|=( COMP_EXTI_LINE_COMP2)
#define __hardware_comp2_reset_rising_edge_irq()  	 				EXTI->RTSR&=(~COMP_EXTI_LINE_COMP2)
#define __hardware_comp2_set_rising_edge_irq_only()				 {__hardware_comp2_set_rising_edge_irq();__hardware_comp2_reset_falling_edge_irq();}
#define __hardware_comp2_set_falling_edge_irq_only()			 {__hardware_comp2_reset_rising_edge_irq();__hardware_comp2_set_falling_edge_irq();}
#define __hardware_comp2_set_both_edges_irq()							 {__hardware_comp2_set_falling_edge_irq();__hardware_comp2_set_rising_edge_irq();}

#define __hardware_comp_i_peak_set_falling_edge_irq() 	 		__hardware_comp2_set_falling_edge_irq()
#define __hardware_comp_i_peak_reset_falling_edge_irq() 		__hardware_comp2_reset_falling_edge_irq()
#define __hardware_comp_i_peak_set_rising_edge_irq()		 		__hardware_comp2_set_rising_edge_irq()
#define __hardware_comp_i_peak_reset_rising_edge_irq()  	 	__hardware_comp2_reset_rising_edge_irq()
#define __hardware_comp_i_peak_set_rising_edge_irq_only()		__hardware_comp2_set_rising_edge_irq_only()
#define __hardware_comp_i_peak_set_falling_edge_irq_only()	__hardware_comp2_set_falling_edge_irq_only()
#define __hardware_comp_i_peak_set_both_edges_irq()					__hardware_comp2_set_both_edges_irq()

#define __hardware_io_ext_set()			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_14,GPIO_PIN_SET)
#define __hardware_io_ext_reset()		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_14,GPIO_PIN_RESET)
#define __hardware_io_ext_toggle()	HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_14)


#define __hardware_bemf_attenuation_enable() 	{GLOBAL_SET_PIN_AS_OUTPUT(GPIOC,12);HAL_GPIO_WritePin(GPIOC,GPIO_PIN_12,GPIO_PIN_SET);}
#define __hardware_bemf_attenuation_disable()	{GLOBAL_SET_PIN_AS_OUTPUT(GPIOC,12);HAL_GPIO_WritePin(GPIOC,GPIO_PIN_12,GPIO_PIN_RESET);}
#define __hardware_bemf_attenuation_auto()		{GLOBAL_SET_PIN_AS_HIZ(GPIOC,12);}

#define __HARDWARE_BEMF_ATT_ENABLE	0
#define __HARDWARE_BEMF_ATT_DISABLE 1
#define __HARDWARE_BEMF_ATT_AUTO		2

uint32_t hardware_bemf_attenuation_select(uint32_t __hardware_bemf_attenuation_mode);





#define __hardware_exti0_disable_irq()						    HAL_NVIC_DisableIRQ(EXTI0_1_IRQn)
#define __hardware_exti0_enable_irq()  							  HAL_NVIC_EnableIRQ(EXTI0_1_IRQn)
#define __hardware_exti0_set_falling_edge_irq() 	 		EXTI->FTSR|=( GPIO_PIN_0)
#define __hardware_exti0_reset_falling_edge_irq() 		EXTI->FTSR&=(~GPIO_PIN_0)
#define __hardware_exti0_set_rising_edge_irq()		 		EXTI->RTSR|=( GPIO_PIN_0)
#define __hardware_exti0_reset_rising_edge_irq()  	 	EXTI->RTSR&=(~GPIO_PIN_0)

#define __hardware_exti0_set_rising_edge_irq_only()		{__hardware_exti0_set_rising_edge_irq();__hardware_exti0_reset_falling_edge_irq();}
#define __hardware_exti0_set_falling_edge_irq_only()	{__hardware_exti0_reset_rising_edge_irq();__hardware_exti0_set_falling_edge_irq();}
#define __hardware_exti0_set_both_edges_irq()					{__hardware_exti0_set_rising_edge_irq();__hardware_exti0_set_falling_edge_irq();}

//

#define __hardware_exti4_15_disable_irq()						    HAL_NVIC_DisableIRQ(EXTI4_15_IRQn)
#define __hardware_exti4_15_enable_irq()  							HAL_NVIC_EnableIRQ(EXTI4_15_IRQn)
#define __hardware_exti15_set_falling_edge_irq() 	 		EXTI->FTSR|=( GPIO_PIN_15)
#define __hardware_exti15_reset_falling_edge_irq() 		EXTI->FTSR&=(~GPIO_PIN_15)
#define __hardware_exti15_set_rising_edge_irq()		 		EXTI->RTSR|=( GPIO_PIN_15)
#define __hardware_exti15_reset_rising_edge_irq()  	 	EXTI->RTSR&=(~GPIO_PIN_15)

#define __hardware_exti15_set_rising_edge_irq_only()	{__hardware_exti15_set_rising_edge_irq();__hardware_exti15_reset_falling_edge_irq();}
#define __hardware_exti15_set_falling_edge_irq_only()	{__hardware_exti15_reset_rising_edge_irq();__hardware_exti15_set_falling_edge_irq();}
#define __hardware_exti15_set_both_edges_irq()				{__hardware_exti15_set_rising_edge_irq();__hardware_exti15_set_falling_edge_irq();}






#define __hardware_pwm_irq_at_end_pulse_enable()		 		 __hardware_exti4_15_enable_irq()
#define __hardware_pwm_irq_at_end_pulse_disable()				 __hardware_exti4_15_disable_irq()
#define __hardware_pwm_set_irq_at_end_toff_only()			   __hardware_exti15_set_rising_edge_irq_only()
#define __hardware_pwm_set_irq_at_end_ton_only()				 __hardware_exti15_set_falling_edge_irq_only()
#define __hardware_pwm_set_irq_at_end_ton_and_toff()	   __hardware_exti15_set_both_edges_irq()		
#define __hardware_pwm_reset_irq_at_end_of_pulse()	  	{__hardware_exti15_reset_falling_edge_irq();__hardware_exti15_reset_rising_edge_irq();}


//----------------------------------------------------					
//Timer 2: ZCD BEMF BLANKING -  tick de 500nS	con IRQ
//----------------------------------------------------			
#define __hardware_tim2_set_irq_in_half_us_unit(time_in_500ns_tick) {\
			__HAL_TIM_SET_AUTORELOAD(&htim2,time_in_500ns_tick);\
		__HAL_TIM_SET_COUNTER(&htim2,1);\
		__HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);\
			HAL_TIM_Base_Start_IT(&htim2);\
		}

#define __hardware_tim2_set_irq_in_us(time_in_us) __hardware_tim2_set_irq_in_half_us_unit((time_in_us<<1))
		
#define	__hardware_tim2_abort() HAL_TIM_Base_Stop_IT(&htim2)		
#define __hardware_tim2_get_count() 	__HAL_TIM_GET_COUNTER(&htim2)
#define __hardware_tim2_get_count_us()	(__hardware_tim2_get_count()>>1)		
		
		
		
//----------------------------------------------------	
//Timer 3: ZCD Y PERIODO  -     tick de 500nS con IRQ
//----------------------------------------------------																								
#define __hardware_tim3_set_irq_in_half_us_unit(time_in_500ns_tick) {\
			__HAL_TIM_SET_AUTORELOAD(&htim3,time_in_500ns_tick);\
			__HAL_TIM_SET_COUNTER(&htim3,1);\
			__HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE);\
			HAL_TIM_Base_Start_IT(&htim3);\
		}

#define __hardware_tim3_set_irq_in_us(time_in_us) __hardware_tim3_set_irq_in_half_us_unit(((time_in_us)<<1))
				
#define	__hardware_tim3_abort() HAL_TIM_Base_Stop_IT(&htim3)

#define __hardware_tim3_get_count() 	__HAL_TIM_GET_COUNTER(&htim3)
#define __hardware_tim3_get_count_us()	(__hardware_tim3_get_count()>>1)
		
		
		
//----------------------------------------------------	
//Timer 15: ZCD Y PERIODO  -     tick de 500nS con IRQ
//----------------------------------------------------																								
#define __hardware_tim15_set_irq_in_half_us_unit(time_in_500ns_tick) {\
			__HAL_TIM_SET_AUTORELOAD(&htim15,time_in_500ns_tick);\
			__HAL_TIM_SET_COUNTER(&htim15,1);\
			__HAL_TIM_CLEAR_IT(&htim15, TIM_IT_UPDATE);\
			HAL_TIM_Base_Start_IT(&htim15);\
		}

#define __hardware_tim15_set_irq_in_us(time_in_us) __hardware_tim15_set_irq_in_half_us_unit(((time_in_us)<<1))
				
#define	__hardware_tim15_abort() HAL_TIM_Base_Stop_IT(&htim15)

#define __hardware_tim15_get_count() 	__HAL_TIM_GET_COUNTER(&htim15)
#define __hardware_tim15_get_count_us()	(__hardware_tim15_get_count()>>1)		
		
		
		
		
static void pcb_hardware_pwm_config(void);                              
static void pcb_hardware_gpios_outputs_pp_config(void);
static void pcb_hardware_gpios_inputs_config(void);
static void pcb_hardware_gpios_hiz_config(void);
static void pcb_hardware_uart_config(void);
static void pcb_hardware_dma_config(void);
static void pcb_hardware_adc_config(void);
static void pcb_hardware_comp_config(void);
void hardware_gpios_edge_events_config(void);
void hardware_tim2_config(void);
void hardware_tim3_config(void);
void hardware_tim15_config(void);
		
uint32_t is_the_value_within_the_band(uint32_t value,uint32_t center_value,uint32_t tolerance_in_2_pow);
