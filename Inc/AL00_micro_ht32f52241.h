#ifndef AL00_MICRO_HT32F52241_H

#define AL00_MICRO_HT32F52241_H

#include "ht32.h"
#include "ht32_board.h"
#include "ht32_board_config.h"
#include "AL00_common_functions.h"

/*******************************************/
/*        	 PARA INTERFACEAR			 			   */
/*******************************************/
//-------------------------------------------
// 					Generales
//-------------------------------------------
void micro_config (void);


//-------------------------------------------
//						SYSTICK
//-------------------------------------------
uint32_t Systick_GetTick(void);

#define __hardware_get_tick_timer()	Systick_GetTick()

//-------------------------------------------
//						GPIOS
//-------------------------------------------
int32_t 	hardware_gpio_event_rising_event_link_callback	(void (*func_pointer)(void), uint32_t exti_line);
int32_t 	hardware_gpio_event_falling_event_link_callback	(void (*func_pointer)(void), uint32_t exti_line);


void hardware_gpio_config_output_pp_pins_load_config		(HT_GPIO_TypeDef *GPIOx,uint32_t pins);
void hardware_gpio_config_hiz_pins_load_config					(HT_GPIO_TypeDef *GPIOx,uint32_t pins);
void hardware_gpio_config_edge_events_pins_load_config	(HT_GPIO_TypeDef *GPIOx,uint32_t pins);
void hardware_gpio_config_inputs_pins_load_config				(HT_GPIO_TypeDef *GPIOx,uint32_t pins);

#define GPIO0				0
#define GPIO1				1
#define	GPIO2				2
#define GPIO3				3
#define GPIO4				4
#define GPIO5				5
#define GPIO6				6
#define GPIO7				7
#define GPIO8				8
#define GPIO9				9
#define GPIO10			10
#define GPIO11			11
#define GPIO12			12
#define GPIO13			13
#define GPIO14			14
#define GPIO15			15



#if defined HT_GPIOA
#define GPIOA								HT_GPIOA
#define GPIO_CNT						1
#endif

#if defined HT_GPIOB
#define GPIOB								HT_GPIOB
#undef	GPIO_CNT	
#define GPIO_CNT						2
#endif

#if defined HT_GPIOC
#define GPIOC								HT_GPIOC
#undef	GPIO_CNT	
#define GPIO_CNT						3
#endif

#if defined HT_GPIOD
#define GPIOD								HT_GPIOD
#undef	GPIO_CNT	
#define GPIO_CNT						4
#endif


#define __hardware_gpio_output_reset(gpio_port,GPIOnum)												(gpio_port->RR     = (1<<GPIOnum))
#define __hardware_gpio_output_set(gpio_port,GPIOnum)													(gpio_port->SRR    = (1<<GPIOnum))
#define __hardware_gpio_output_toggle(gpio_port,GPIOnum)											(gpio_port->DOUTR ^= (1<<GPIOnum))

#define __hardware_gpio_input_read_state(gpio_port,GPIOnum)										(GPIO_ReadInBit(gpio_port,(1<<GPIOnum)))
#define __hardware_gpio_read_entire_port(gpio_port)														(gpio_port->DINR)

#define __hardware_gpio_config_set_as_hiz_pin(gpio_port,GPIOnum)							(gpio_port->DIRCR &= ~(1<<GPIOnum))
#define __hardware_gpio_config_set_as_pushpull_output_pin(gpio_port,GPIOnum)	(gpio_port->DIRCR |= (1<<GPIOnum))


#define __hardware_gpio_event_detection_disable(GPIOnum)											(HT_EXTI->CR 				&= ~(1 << GPIOnum))

#define __hardware_gpio_event_rising_only_detection_enable(GPIOnum)						{(HT_EXTI->EDGEFLGR 								|=	(1 << GPIOnum));\
																																							 (HT_EXTI->EDGESR										|=	(1 << GPIOnum));\
																																							 (*((&HT_EXTI->CFGR0)+GPIOnum)			&= ~(7 << 28));\
																																							 (*((&HT_EXTI->CFGR0)+GPIOnum)			|=	(3 << 28));\
																																							 (HT_EXTI->CR												|=	(1 << GPIOnum));}


#define __hardware_gpio_event_falling_only_detection_enable(GPIOnum)					{(HT_EXTI->EDGEFLGR 								|=	(1 << GPIOnum));\
																																							 (HT_EXTI->EDGESR										|=	(1 << GPIOnum));\
																																							 (*((&HT_EXTI->CFGR0)+GPIOnum)			&= ~(7 << 28));\
																																							 (*((&HT_EXTI->CFGR0)+GPIOnum)			|=	(2 << 28));\
																																							 (HT_EXTI->CR												|=	(1 <<GPIOnum));}																																										 

#define __hardware_gpio_event_both_edges_detection_enable(GPIOnum)						{(HT_EXTI->EDGEFLGR 								|=	(1 << GPIOnum));\
																																							 (HT_EXTI->EDGESR										|=	(1 << GPIOnum));\
																																							 (*((&HT_EXTI->CFGR0)+GPIOnum)			&= ~(7 << 28));\
																																							 (*((&HT_EXTI->CFGR0)+GPIOnum)			|=	(4 << 28));\
																																							 (HT_EXTI->CR												|=	(1 <<GPIOnum));}
																																							 
//-------------------------------------------
//				 TIMERS
//-------------------------------------------
#define __HARDWARE_TIM_BFTM0_SEL 0
#define __HARDWARE_TIM_BFTM1_SEL 1
#define __HARDWARE_TIM_SCTM0_SEL 2
#define __HARDWARE_TIM_SCTM1_SEL 3

#define HT_TIM_BFTM0	HT_BFTM0
#define HT_TIM_BFTM1	HT_BFTM1		
#define HT_TIM_SCTM0	HT_SCTM0																																						 
#define HT_TIM_SCTM1	HT_SCTM1
																																							 
int32_t hardware_htim_link_callback (void (*func_pointer)(void), uint32_t __hardware_tim_sel);

#define __harwdare_tim_bftm_init_timer_with_timeout_irq_us(HT_TIM_BFTMx,time_us)	{\
																					BFTM_EnaCmd(HT_TIM_BFTMx, DISABLE);\
																					BFTM_IntConfig(HT_TIM_BFTMx, DISABLE);\
																					BFTM_SetCounter(HT_TIM_BFTMx, 0);\
																					BFTM_SetCompare(HT_TIM_BFTMx, ((time_us<<5)+(time_us<<3)));\
																					BFTM_IntConfig(HT_TIM_BFTMx, ENABLE);\
																				  BFTM_EnaCmd(HT_TIM_BFTMx, ENABLE);\
																					}


#define __harwdare_tim_bftm_abort(HT_TIM_BFTMx)		{BFTM_EnaCmd(HT_TIM_BFTMx, DISABLE);BFTM_IntConfig(HT_TIM_BFTMx,DISABLE);BFTM_SetCounter(HT_TIM_BFTMx,0);}
																																																																														
#define __hardware_tim_bftm_get_count_us(HT_TIM_BFTMx)	(BFTM_GetCounter(HT_TIM_BFTMx))



#define __hardware_tim_sctm_init_timer_with_timeout_irq_us(HT_TIM_SCTMx,time_us)	{\
																					TM_Cmd(HT_TIM_SCTMx,DISABLE);\
																					TM_IntConfig(HT_TIM_SCTMx,TM_INT_UEV,DISABLE);	\
																					TM_SetCounter(HT_TIM_SCTMx,0);\
																					TM_SetCounterReload(HT_TIM_SCTMx,time_us);\
																					TM_IntConfig(HT_TIM_SCTMx,TM_INT_UEV,ENABLE);	\
																				  TM_Cmd(HT_TIM_SCTMx,ENABLE);\
																					}

#define __harwdare_tim_sctm_abort(HT_TIM_SCTMx)		{TM_Cmd(HT_TIM_SCTMx,DISABLE);TM_IntConfig(HT_TIM_SCTMx,TM_INT_UEV,DISABLE);TM_SetCounter(HT_TIM_SCTMx,0);}

#define __hardware_tim_sctm_get_count_us(HT_TIM_SCTMx) (TM_GetCounter(HT_TIM_SCTMx))




//-------------------------------------------
//						UART
//-------------------------------------------
uint8_t hardware_uart_read 		(void);
#define hardware_uart_transmit	printf

//-------------------------------------------
//				 PWM PRINCIPAL
//-------------------------------------------
int32_t hardware_pwm_set_period_us 									(int32_t period_us);
int32_t hardware_pwm_get_period_us 									(void);
int32_t hardware_pwm_set_ton_us		 									(int32_t ton_us);
int32_t hardware_pwm_get_ton_us		 									(void);
int32_t hardware_pwm_set_toff_us										(int32_t toff_us);
int32_t harwdare_pwm_get_toff_us										(void);
int32_t hardware_pwm_set_sense_on_toff_before_ton_us(int32_t time_us);
void    hardware_pwm_start_with_ton									(void);
void 		hardware_pwm_set_outputs_to_toff						(void);
int32_t hardware_pwm_get_ton_elapsed								(void);

void		hardware_pwm_end_toff_link_callback 				(void (*func_pointer)(void));
void 		hardware_pwm_break_function_link_callback		(void (*func_pointer)(void));

#define __hardware_pwm_hin1_enable()								TM_ChannelConfig(HT_MCTM0, TM_CH_1, TM_CHCTL_ENABLE)
#define __hardware_pwm_hin1_disable()								TM_ChannelConfig(HT_MCTM0, TM_CH_1, TM_CHCTL_DISABLE)
#define __hardware_pwm_hin2_enable()								TM_ChannelConfig(HT_MCTM0, TM_CH_3, TM_CHCTL_ENABLE)
#define __hardware_pwm_hin2_disable()								TM_ChannelConfig(HT_MCTM0, TM_CH_3, TM_CHCTL_DISABLE)
#define __hardware_pwm_hin3_enable()								TM_ChannelConfig(HT_MCTM0, TM_CH_0, TM_CHCTL_ENABLE)
#define __hardware_pwm_hin3_disable()								TM_ChannelConfig(HT_MCTM0, TM_CH_0, TM_CHCTL_DISABLE)

#define __hardware_pwm_end_toff_disable_irq()				{TM_IntConfig(HT_MCTM0,TM_FLAG_CH2CC,DISABLE)			; TM_ClearFlag(HT_MCTM0,TM_FLAG_CH2CC);}
#define __hardware_pwm_end_toff_enable_irq()				{TM_IntConfig(TM_ClearFlag(HT_MCTM0,TM_FLAG_CH2CC); HT_MCTM0,TM_FLAG_CH2CC,ENABLE);}

#define __hardware_pwm_break_disable_irq()					{TM_IntConfig(HT_MCTM0,TM_FLAG_BRK0,DISABLE)			; TM_ClearFlag(HT_MCTM0,TM_FLAG_BRK0);}
#define __hardware_pwm_break_enable_irq()						{TM_ClearFlag(HT_MCTM0,TM_FLAG_BRK0)							;	TM_IntConfig(HT_MCTM0,TM_FLAG_BRK0,ENABLE);}


#endif /* AL00_MICRO_HT32F52241_H */
