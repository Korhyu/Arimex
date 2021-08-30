#include "AL00_micro_ht32f52241.h"
#include "Jose.h"


//-------------------------------------------
//						SYSTICK
//-------------------------------------------
static uint32_t TickCount=0;


//-------------------------------------------
//						GPIOS
//-------------------------------------------
#define GPIOx_SEL_COUNT 		GPIO_CNT

#if GPIO_CNT >= 1
#define	GPIOA_SEL						0
#define INIT_GPIO_PORTS_ARRAY	{GPIOA}
#endif

#if GPIO_CNT >= 2
#define	GPIOB_SEL						1
#undef 	INIT_GPIO_PORTS_ARRAY
#define INIT_GPIO_PORTS_ARRAY	{GPIOA,GPIOB}
#endif

#if GPIO_CNT >= 3
#define	GPIOC_SEL						2
#undef 	INIT_GPIO_PORTS_ARRAY
#define INIT_GPIO_PORTS_ARRAY	{GPIOA,GPIOB,GPIOC}
#endif

#if GPIO_CNT >= 4
#define GPIOD_SEL 					3
#undef 	INIT_GPIO_PORTS_ARRAY
#define INIT_GPIO_PORTS_ARRAY	{GPIOA,GPIOB,GPIOC,GPIOD}
#endif


static HT_GPIO_TypeDef* GPIO_PORTs[GPIOx_SEL_COUNT]=INIT_GPIO_PORTS_ARRAY;

//static HT_GPIO_TypeDef* GPIO_port_of_edge_event[16];	//Para cada linea Exti de la 0 a la 15 se le asocia un puerto para evaluar el pin en la IRQ

static uint32_t gpio_config_output_pp_pins[GPIOx_SEL_COUNT];
static uint32_t gpio_config_hiz_pins[GPIOx_SEL_COUNT];
static uint32_t gpio_config_edge_events_pins[GPIOx_SEL_COUNT];
static uint32_t gpio_config_inputs_pins[GPIOx_SEL_COUNT];

void hardware_gpio_config_output_pp_pins_load_config(HT_GPIO_TypeDef *GPIOx,uint32_t pins);
void hardware_gpio_config_hiz_pins_load_config(HT_GPIO_TypeDef *GPIOx,uint32_t pins);
void hardware_gpio_config_edge_events_pins_load_config(HT_GPIO_TypeDef *GPIOx,uint32_t pins);
void hardware_gpio_config_inputs_pins_load_config(HT_GPIO_TypeDef *GPIOx,uint32_t pins);


static void micro_gpios_outputs_pp_config(void);
static void micro_gpios_inputs_pullup_config(void);
static void micro_gpios_hiz_config(void);
static void micro_gpios_edge_events_config(void);

static void micro_pwm_config(void);

static void micro_timers_config(void);

static void micro_adc_config(void);

void empty_func_to_avoid_hard_fault(void){}

#define EXTI_LINE_COUNT 	16
//Contiene las direcciones de las funciones que se van a llamar
//Se cargan con
void (*func_ptr_callback_rising_exti [EXTI_LINE_COUNT])(void) = {empty_func_to_avoid_hard_fault,empty_func_to_avoid_hard_fault,
																 empty_func_to_avoid_hard_fault,empty_func_to_avoid_hard_fault,
																 empty_func_to_avoid_hard_fault,empty_func_to_avoid_hard_fault,
																 empty_func_to_avoid_hard_fault,empty_func_to_avoid_hard_fault,
																 empty_func_to_avoid_hard_fault,empty_func_to_avoid_hard_fault,
																 empty_func_to_avoid_hard_fault,empty_func_to_avoid_hard_fault,
																 empty_func_to_avoid_hard_fault,empty_func_to_avoid_hard_fault,
																 empty_func_to_avoid_hard_fault,empty_func_to_avoid_hard_fault};

void (*func_ptr_callback_falling_exti[EXTI_LINE_COUNT])(void) = {empty_func_to_avoid_hard_fault,empty_func_to_avoid_hard_fault,
																 empty_func_to_avoid_hard_fault,empty_func_to_avoid_hard_fault,
																 empty_func_to_avoid_hard_fault,empty_func_to_avoid_hard_fault,
																 empty_func_to_avoid_hard_fault,empty_func_to_avoid_hard_fault,
																 empty_func_to_avoid_hard_fault,empty_func_to_avoid_hard_fault,
																 empty_func_to_avoid_hard_fault,empty_func_to_avoid_hard_fault,
																 empty_func_to_avoid_hard_fault,empty_func_to_avoid_hard_fault,
																 empty_func_to_avoid_hard_fault,empty_func_to_avoid_hard_fault};


																 

//-------------------------------------------
//				 TIMERS
//-------------------------------------------
void (*func_ptr_callback_bftm0)(void) = empty_func_to_avoid_hard_fault;
void (*func_ptr_callback_bftm1)(void) = empty_func_to_avoid_hard_fault;
void (*func_ptr_callback_sctm0)(void) = empty_func_to_avoid_hard_fault;
void (*func_ptr_callback_sctm1)(void) = empty_func_to_avoid_hard_fault;																 

																 
																 
//-------------------------------------------
//				 PWM
//-------------------------------------------					
static int32_t pwm_time_before_ton_start_us=10;																 
void (*func_ptr_callback_pwm_end_toff)(void) = empty_func_to_avoid_hard_fault;			
void (*func_ptr_callback_pwm_break)(void) 	 = empty_func_to_avoid_hard_fault;
																 
																 
																 

																 
uint32_t Systick_GetTick(void)
{
	return  TickCount;
}

/*********************************************************************************************************//**
 * @brief   This function handles SysTick Handler.
 * @retval  None
 ************************************************************************************************************/
void SysTick_Handler(void)
{
	TickCount++;
}




void hardware_gpio_config_output_pp_pins_load_config(HT_GPIO_TypeDef *GPIOx,uint32_t pins)
{
	uint32_t i=0;
	
	for(i=0;i<GPIOx_SEL_COUNT;i++)
	{
		if(GPIOx == GPIO_PORTs[i])
		{
			gpio_config_output_pp_pins[i] = pins;
			i = GPIOx_SEL_COUNT;
		}
	}
}

void hardware_gpio_config_hiz_pins_load_config(HT_GPIO_TypeDef *GPIOx,uint32_t pins)
{
	uint32_t i=0;
	for(i=0;i<GPIOx_SEL_COUNT;i++)
	{
		if(GPIOx == GPIO_PORTs[i])
		{
			gpio_config_hiz_pins[i] = pins;
			i = GPIOx_SEL_COUNT;
		}
	}	
	
}	

void hardware_gpio_config_edge_events_pins_load_config(HT_GPIO_TypeDef *GPIOx,uint32_t pins)
{
	uint32_t i=0;
	for(i=0;i<GPIOx_SEL_COUNT;i++)
	{
		if(GPIOx == GPIO_PORTs[i])
		{
			gpio_config_edge_events_pins[i] = pins;
			i = GPIOx_SEL_COUNT;
		}
	}
}

void hardware_gpio_config_inputs_pins_load_config(HT_GPIO_TypeDef *GPIOx,uint32_t pins)
{
	uint32_t i=0;
	for(i=0;i<GPIOx_SEL_COUNT;i++)
	{
		if(GPIOx == GPIO_PORTs[i])
		{
			gpio_config_inputs_pins[i] = pins;
			i = GPIOx_SEL_COUNT;
		}
	}
}




//Activa clocks de los puertos
static void _HT32F_DVB_ClockConfig(u32 GpioId)
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  u8 RegCK[GPIO_PORT_NUM] = {0};
  RegCK[GpioId] = 1;

  CKCUClock.Bit.PA         = RegCK[0];
  CKCUClock.Bit.PB         = RegCK[1];
  #if defined(LIBCFG_GPIOC)
  CKCUClock.Bit.PC         = RegCK[2];
  #endif
  #if defined(LIBCFG_GPIOD)
  CKCUClock.Bit.PD         = RegCK[3];
  #endif
  #if defined(LIBCFG_GPIOE)
  CKCUClock.Bit.PE         = RegCK[4];
  #endif
  #if defined(LIBCFG_GPIOF)
  CKCUClock.Bit.PF         = RegCK[5];
  #endif
  CKCUClock.Bit.AFIO       = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
}


static void micro_gpios_outputs_pp_config(void)
{
	uint32_t gpio_port_sel_index=0,gpio_pin_index;
	
	do{ //Recorro los puertos 
	
		_HT32F_DVB_ClockConfig(gpio_port_sel_index);/* GPIO Ports Clock Enable */
		
		if(gpio_config_output_pp_pins[gpio_port_sel_index] != 0) //Si hay pines selccionados en el GPIOx en cuestion, lo configuro, sino paso al siguiente GPIOx
		{							
				gpio_pin_index=1;
			
				do{ //Recorro los pines
					
							if(gpio_config_output_pp_pins[gpio_port_sel_index] & gpio_pin_index)
							{
								AFIO_GPxConfig				 (gpio_port_sel_index, gpio_pin_index , AFIO_FUN_GPIO);
								
								GPIO_PullResistorConfig(GPIO_PORTs[gpio_port_sel_index], gpio_pin_index, GPIO_PR_DISABLE);
								GPIO_DriveConfig			 (GPIO_PORTs[gpio_port_sel_index], gpio_pin_index, GPIO_DV_16MA);		
								GPIO_DirectionConfig	 (GPIO_PORTs[gpio_port_sel_index], gpio_pin_index, GPIO_DIR_OUT);
							}
							gpio_pin_index<<=1;
							
				}while(gpio_pin_index<(1<<16));
		
		}
		gpio_port_sel_index++;
	}while(gpio_port_sel_index<GPIOx_SEL_COUNT);
}



static void micro_gpios_inputs_pullup_config(void)
{
	uint32_t gpio_port_sel_index=0,gpio_pin_index;

	do{ //Recorro los puertos 
		
		if(gpio_config_inputs_pins[gpio_port_sel_index] != 0) //Si hay pines selccionados en el GPIOx en cuestion, lo configuro, sino paso al siguiente GPIOx
		{							
				gpio_pin_index=1;
			
				do{ //Recorro los pines
					
							if(gpio_config_inputs_pins[gpio_port_sel_index] & gpio_pin_index)
							{
									AFIO_GPxConfig					(gpio_port_sel_index, gpio_pin_index, AFIO_FUN_GPIO);
								
									GPIO_DirectionConfig		(GPIO_PORTs[gpio_port_sel_index], gpio_pin_index, GPIO_DIR_IN);
									GPIO_PullResistorConfig	(GPIO_PORTs[gpio_port_sel_index], gpio_pin_index, GPIO_PR_UP);
									GPIO_InputConfig				(GPIO_PORTs[gpio_port_sel_index], gpio_pin_index, ENABLE);
							}
							gpio_pin_index<<=1;
							
				}while(gpio_pin_index<(1<<16));
		}
		gpio_port_sel_index++;
	}while(gpio_port_sel_index<GPIOx_SEL_COUNT);
}



static void micro_gpios_hiz_config(void)
{
	uint32_t gpio_port_sel_index=0,gpio_pin_index;

	do{ //Recorro los puertos 
		
		if(gpio_config_hiz_pins[gpio_port_sel_index] != 0) //Si hay pines selccionados en el GPIOx en cuestion, lo configuro, sino paso al siguiente GPIOx
		{							
				gpio_pin_index=1;
			
				do{ //Recorro los pines
					
							if(gpio_config_hiz_pins[gpio_port_sel_index] & gpio_pin_index)
							{
									AFIO_GPxConfig					(gpio_port_sel_index, gpio_pin_index, AFIO_FUN_GPIO);
								
									GPIO_DirectionConfig		(GPIO_PORTs[gpio_port_sel_index], gpio_pin_index, GPIO_DIR_IN);
									GPIO_PullResistorConfig	(GPIO_PORTs[gpio_port_sel_index], gpio_pin_index, GPIO_PR_DISABLE);
									GPIO_DriveConfig				(GPIO_PORTs[gpio_port_sel_index], gpio_pin_index, GPIO_DV_16MA);
							}
							gpio_pin_index<<=1;
							
				}while(gpio_pin_index<(1<<16));
		}
		gpio_port_sel_index++;
	}while(gpio_port_sel_index<GPIOx_SEL_COUNT);	
}


static void micro_gpios_edge_events_config(void)
{
	uint32_t gpio_port_sel_index=0,gpio_pin_index=1,exti_line_index=0;
	uint32_t gpio_event_pins=0;
	
	EXTI_InitTypeDef EXTI_InitStruct = {0};
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
			
  /* Enable the EXTI Clock                                                                                */
  CKCUClock.Bit.EXTI       = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
	
	
	do{ //Recorro los puertos 	
		if(gpio_config_edge_events_pins[gpio_port_sel_index] != 0) //Si hay pines selccionados en el GPIOx en cuestion, lo configuro, sino paso al siguiente GPIOx
		{							
				gpio_pin_index=1;
				exti_line_index=0;
			
			
				do{ //Recorro los pines
					
							if(gpio_config_edge_events_pins[gpio_port_sel_index] & gpio_pin_index)
							{
									gpio_event_pins |= gpio_pin_index;
								
								
									//Lo siguiente es para linkear que puerto corresponde a cada pin. OJO: si hay multiples numeros de pin repetido va a quedar
									//el ultimo puerto con el pin puesto (iniciando por GPIOA yendo hacia GPIOD)
//									GPIO_port_of_edge_event[exti_line_index]=GPIO_PORTs[gpio_port_sel_index];
								
									AFIO_GPxConfig					(gpio_port_sel_index, gpio_pin_index, AFIO_FUN_GPIO);
														
									GPIO_DirectionConfig		(GPIO_PORTs[gpio_port_sel_index], gpio_pin_index, GPIO_DIR_IN);
									GPIO_PullResistorConfig	(GPIO_PORTs[gpio_port_sel_index], gpio_pin_index, GPIO_PR_DISABLE);
									GPIO_InputConfig				(GPIO_PORTs[gpio_port_sel_index], gpio_pin_index, ENABLE);
								
									
									AFIO_EXTISourceConfig((AFIO_EXTI_CH_Enum)exti_line_index, (AFIO_ESS_Enum)gpio_port_sel_index);// Esto es para asigar el puerto a la linea de exti	
																																																								// Ya que, por ejemplo, hay que definir de todos los pines "0"
																																																								// cual es el puerto que va a estar enlazado a la linea EXTI
																																																								// la EXTI0 puede estar linkeada a PA0, PB0, PC0, PD0 o PE0
								
								  EXTI_InitStruct.EXTI_Channel = exti_line_index;
									EXTI_InitStruct.EXTI_Debounce = EXTI_DEBOUNCE_DISABLE;
									EXTI_InitStruct.EXTI_DebounceCnt = 0;
									EXTI_InitStruct.EXTI_IntType = EXTI_POSITIVE_EDGE;
									EXTI_Init(&EXTI_InitStruct);	
							}
							
							gpio_pin_index<<=1;
							exti_line_index+=1;	
				}while(gpio_pin_index<(1<<16));
		}
		
		gpio_port_sel_index++;
	}while(gpio_port_sel_index<GPIOx_SEL_COUNT);
	
	
	//Lo siguiente es Habilitar las IRQ en funcion de los pines (independientmenete del puerto) que se hayan seleccionado
	gpio_pin_index=1;
	exti_line_index=0;
	
	do{
		if(gpio_pin_index&gpio_event_pins)
		{			
			switch(gpio_pin_index)
			{
				case (1<<0):	
				case (1<<1):	NVIC_EnableIRQ(EXTI0_1_IRQn);
											gpio_pin_index = (1<<1);
											break;
				case (1<<2):
				case (1<<3):	NVIC_EnableIRQ(EXTI2_3_IRQn);
											gpio_pin_index = (1<<3);
											break;
				case (1<<4):
				case (1<<5):
				case (1<<6):
				case (1<<7):
				case (1<<8):
				case (1<<9):
				case (1<<10):
				case (1<<11):
				case (1<<12):
				case (1<<13):
				case (1<<14):
				case (1<<15):	NVIC_EnableIRQ(EXTI4_15_IRQn);
											gpio_pin_index = (1<<15);
											break;				
			}
		}
		gpio_pin_index<<=1;			
		exti_line_index+=1;
	}while(gpio_pin_index<=(1<<15));
}




void SYSTICK_Configuration(void);
void NVIC_Configuration(void);
void CKCU_Configuration(void);
void GPIO_Configuration(void);
#if (ENABLE_CKOUT == 1)
void CKOUTConfig(void);
#endif






																 
//-------------------------------------------
//				 PWM
//-------------------------------------------																		 
void hardware_pwm_end_toff_link_callback (void (*func_pointer)(void))
{
	func_ptr_callback_pwm_end_toff=func_pointer;
}


void hardware_pwm_break_function_link_callback(void (*func_pointer)(void))
{
	func_ptr_callback_pwm_break=func_pointer;
}


static void micro_pwm_config(void)
{
	/*	HT32F52231/41
			 
			PB15    PA14    PC1  	      |		CH0
			PA10		PB0     PB7    	    | 	CH1
			PC6     PB2								  | 	CH2  
			PB9									        | 	CH3
																	|
			PB4			PC3		    					|		BREAK
	*/
	
	TM_TimeBaseInitTypeDef 	 TM_TimeBaseInitStructure;
  TM_OutputInitTypeDef 		 TM_OutputInitStructure;
	
	MCTM_CHBRKCTRInitTypeDef CHBRKCTRInitStructure;
	
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{ 0 }};
  CKCUClock.Bit.AFIO       = 1;
  CKCUClock.Bit.MCTM0      = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
		
	/* Configure the GPIO as TM channel output AFIO function                                       */
  //AFIO_GPxConfig(GPIO_PA, AFIO_PIN_10, AFIO_FUN_MCTM_GPTM);	//CH1 PA10
	//AFIO_GPxConfig(GPIO_PB, AFIO_PIN_9 , AFIO_FUN_MCTM_GPTM);	//CH3 PB9
	//AFIO_GPxConfig(GPIO_PB, AFIO_PIN_15, AFIO_FUN_MCTM_GPTM);	//CH0 PB15
	
	AFIO_GPxConfig(GPIO_PB, AFIO_PIN_4 , AFIO_FUN_MCTM_GPTM);	//PB4: BREAK PIN
	
	//AFIO_GPxConfig(GPIO_PC, AFIO_PIN_2, AFIO_FUN_MCTM_GPTM);		//CH0 PC2 - LED Built in  - Directo
	AFIO_GPxConfig(GPIO_PB, AFIO_PIN_15, AFIO_FUN_MCTM_GPTM);		//CH0 PB15 - Negado - W
	AFIO_GPxConfig(GPIO_PA, AFIO_PIN_10, AFIO_FUN_MCTM_GPTM);		//CH1 PA10 - Negado - U
	//AFIO_GPxConfig(GPIO_PA, AFIO_PIN_10, AFIO_FUN_MCTM_GPTM);		//CH1 PA11 - Directo
	AFIO_GPxConfig(GPIO_PB, AFIO_PIN_9 , AFIO_FUN_MCTM_GPTM);		//CH3 PB9  - Negado - V
	
	
	TM_TimeBaseInitStructure.CounterReload = 100;
  TM_TimeBaseInitStructure.Prescaler = 40;
  TM_TimeBaseInitStructure.RepetitionCounter = 0;
  TM_TimeBaseInitStructure.CounterMode = TM_CNT_MODE_UP;
  TM_TimeBaseInitStructure.PSCReloadTime = TM_PSC_RLD_IMMEDIATE;
  TM_TimeBaseInit(HT_MCTM0, &TM_TimeBaseInitStructure);
	
  
  TM_OutputInitStructure.OutputMode = TM_OM_PWM2;
  TM_OutputInitStructure.Control = TM_CHCTL_ENABLE;				//TM_CHCTL_DISABLE
  TM_OutputInitStructure.ControlN = TM_CHCTL_DISABLE;			//TM_CHCTL_ENABLE
  TM_OutputInitStructure.Polarity = TM_CHP_NONINVERTED;		//TM_CHP_NONINVERTED
  TM_OutputInitStructure.PolarityN = TM_CHP_INVERTED;			//TM_CHP_INVERTED
  TM_OutputInitStructure.IdleState = MCTM_OIS_LOW;
  TM_OutputInitStructure.IdleStateN = MCTM_OIS_HIGH;			//MCTM_OIS_LOW 
	
	//HIN3 CH0 (PB15) - Directo
	TM_OutputInitStructure.Channel = TM_CH_0;
	TM_OutputInitStructure.Compare = 80;
	TM_OutputInit(HT_MCTM0, &TM_OutputInitStructure);
	//HIN1 CH1 (PA10) - Directo
	TM_OutputInitStructure.Channel = TM_CH_1;
  TM_OutputInitStructure.Compare = 80;
	TM_OutputInit(HT_MCTM0, &TM_OutputInitStructure);
	//HIN2 CH3 (PB9) - Directo
	TM_OutputInitStructure.Channel = TM_CH_3;
	TM_OutputInitStructure.Compare = 80;
	TM_OutputInit(HT_MCTM0, &TM_OutputInitStructure);
	
	//HIN2 CH3 (PB9) - Directo
	TM_OutputInitStructure.Channel = TM_CH_2;
	TM_OutputInitStructure.Compare = 50;
	TM_OutputInit(HT_MCTM0, &TM_OutputInitStructure);
	
	
	TM_ClearFlag(HT_MCTM0,TM_FLAG_CH2CC);				//IRQ de time_before_ton
	NVIC_EnableIRQ(MCTM0_IRQn);
	
	TM_Cmd(HT_MCTM0, ENABLE);
	
	MCTM_CHMOECmd(HT_MCTM0, ENABLE);
	
	TM_ChannelConfig(HT_MCTM0, TM_CH_0, TM_CHCTL_ENABLE);
	TM_ChannelConfig(HT_MCTM0, TM_CH_1, TM_CHCTL_ENABLE);
	TM_ChannelConfig(HT_MCTM0, TM_CH_2, TM_CHCTL_ENABLE);
	TM_ChannelConfig(HT_MCTM0, TM_CH_3, TM_CHCTL_ENABLE);
	
	
	CHBRKCTRInitStructure.AutomaticOutput = MCTM_CHAOE_ENABLE;
	CHBRKCTRInitStructure.Break0 = MCTM_BREAK_ENABLE;
	CHBRKCTRInitStructure.Break0Polarity = MCTM_BREAK_POLARITY_HIGH; 
	CHBRKCTRInitStructure.BreakFilter = 0;
	CHBRKCTRInitStructure.DeadTime = 0;
	CHBRKCTRInitStructure.LockLevel = MCTM_LOCK_LEVEL_OFF;
	CHBRKCTRInitStructure.OSSIState = MCTM_OSSI_STATE_ENABLE;
	CHBRKCTRInitStructure.OSSRState = MCTM_OSSR_STATE_ENABLE;
	
	MCTM_CHBRKCTRConfig(HT_MCTM0,&CHBRKCTRInitStructure);
	
	TM_IntConfig(HT_MCTM0,TM_INT_BRKEV,ENABLE);
	TM_IntConfig(HT_MCTM0,TM_INT_CH2CC,DISABLE);
	TM_IntConfig(HT_MCTM0,TM_INT_UEV,DISABLE);
	//TM_UpdateCmd(HT_MCTM0,DISABLE);
	//TM_ChannelConfig(HT_MCTM0, TM_CH_2, TM_CHCTL_DISABLE);

}


void MCTM0_IRQHandler (void)
{	
	/*
	if(TM_GetFlagStatus(HT_MCTM0,TM_EVENT_CH2CC))
	{
		(*func_ptr_callback_pwm_end_toff)();
			TM_ClearFlag(HT_MCTM0,TM_EVENT_CH2CC);
	}
	*/
	if(TM_GetFlagStatus(HT_MCTM0,TM_FLAG_BRK0))
	{
		(*func_ptr_callback_pwm_break)();	
		TM_ClearFlag(HT_MCTM0,TM_FLAG_BRK0);
		TM_ClearFlag(HT_MCTM0,TM_FLAG_CH2CC);
	}
	
	//if(TM_GetFlagStatus(HT_MCTM0,TM_FLAG_CH2CC) && !TM_GetFlagStatus(HT_MCTM0,TM_FLAG_UEV))
	if(TM_GetFlagStatus(HT_MCTM0,TM_FLAG_CH2CC))
	{
		//__hardware_gpio_output_toggle(GPIOA, 3);					//GPIO aux para monitoreo en OSC
		(*func_ptr_callback_pwm_end_toff)();
		TM_ClearFlag(HT_MCTM0,TM_FLAG_CH2CC);
	}

	/*
	if (TM_GetFlagStatus(HT_MCTM0,TM_FLAG_UEV))
	{
		TM_ClearFlag(HT_MCTM0,TM_FLAG_UEV);
	}
	*/
}

/* Funcion Jose -------------------------------------------------- */
static void micro_modify_PWM (int32_t porc_PWM)
{
	// Carga un nuevo valor para el PWM, el numero esta expresado en porcentaje sin decimales
	float valuePWM = 0;
	TM_OutputInitTypeDef 		 TM_OutputInitStructure;
	
	valuePWM = TM_GetCounter(HT_MCTM0) * porc_PWM / 100;
	
	//HIN3 CH0 (PB15) - Negado
	TM_OutputInitStructure.Channel = TM_CH_0;
	TM_OutputInitStructure.Compare = valuePWM;
	TM_OutputInit(HT_MCTM0, &TM_OutputInitStructure);
	//HIN1 CH1 (PA11) - Directo
	TM_OutputInitStructure.Channel = TM_CH_1;
  TM_OutputInitStructure.Compare = valuePWM;
	TM_OutputInit(HT_MCTM0, &TM_OutputInitStructure);
	//HIN2 CH3 (PB9) - Negado
	TM_OutputInitStructure.Channel = TM_CH_3;
	TM_OutputInitStructure.Compare = valuePWM;
	TM_OutputInit(HT_MCTM0, &TM_OutputInitStructure);
}
/* Funcion Jose -------------------------------------------------- */

int32_t hardware_pwm_set_period_us 	(int32_t period_us)
{
	if(period_us<32000 && period_us>0)
	{
		TM_SetCounterReload(HT_MCTM0,period_us-1);							//Cambio el periodo
	}
	else 
		return -1;
	return 0;
}

int32_t hardware_pwm_get_period_us (void)
{
	return HT_MCTM0->CRR+1;
}	

int32_t hardware_pwm_set_ton_us	(int32_t ton_us)
{
		int32_t return_value=0;
	
		if((ton_us+pwm_time_before_ton_start_us)>=(HT_MCTM0->CRR+1))
		{
			TM_SetCaptureCompare(HT_MCTM0,TM_CH_0, pwm_time_before_ton_start_us +1);
			TM_SetCaptureCompare(HT_MCTM0,TM_CH_1, pwm_time_before_ton_start_us +1);
			TM_SetCaptureCompare(HT_MCTM0,TM_CH_3, pwm_time_before_ton_start_us +1);
			
			TM_SetCaptureCompare(HT_MCTM0,TM_CH_2,1);
			
			return_value=1;
		}
		else
		{
			TM_SetCaptureCompare(HT_MCTM0,TM_CH_0, HT_MCTM0->CRR-ton_us+1);
			TM_SetCaptureCompare(HT_MCTM0,TM_CH_1, HT_MCTM0->CRR-ton_us+1);
			TM_SetCaptureCompare(HT_MCTM0,TM_CH_3, HT_MCTM0->CRR-ton_us+1);
			
			TM_SetCaptureCompare(HT_MCTM0,TM_CH_2, HT_MCTM0->CRR-ton_us - pwm_time_before_ton_start_us + 1);
		}
		return return_value;		
}


int32_t hardware_pwm_get_ton_us		(void)
{
	return(HT_MCTM0->CRR-TM_GetCaptureCompare(HT_MCTM0,TM_CH_0)+1);
}

int32_t hardware_pwm_set_toff_us(int32_t toff_us)
{
		TM_SetCaptureCompare(HT_MCTM0,TM_CH_0, toff_us);
		TM_SetCaptureCompare(HT_MCTM0,TM_CH_1, toff_us);
		TM_SetCaptureCompare(HT_MCTM0,TM_CH_3, toff_us);

		TM_SetCaptureCompare(HT_MCTM0,TM_CH_2, toff_us-pwm_time_before_ton_start_us);
		
		/*
		if (toff_us < pwm_time_before_ton_start_us)
		{
			TM_SetCaptureCompare(HT_MCTM0,TM_CH_0, pwm_time_before_ton_start_us);
			TM_SetCaptureCompare(HT_MCTM0,TM_CH_1, pwm_time_before_ton_start_us);
			TM_SetCaptureCompare(HT_MCTM0,TM_CH_3, pwm_time_before_ton_start_us);

			TM_SetCaptureCompare(HT_MCTM0,TM_CH_2, 1);
		}
		else
		{
			TM_SetCaptureCompare(HT_MCTM0,TM_CH_0, toff_us);
			TM_SetCaptureCompare(HT_MCTM0,TM_CH_1, toff_us);
			TM_SetCaptureCompare(HT_MCTM0,TM_CH_3, toff_us);

			TM_SetCaptureCompare(HT_MCTM0,TM_CH_2,toff_us-pwm_time_before_ton_start_us+1);
		}
		*/

		return 0;
}

int32_t hardware_pwm_get_toff_us(void)
{
	return (TM_GetCaptureCompare(HT_MCTM0,TM_CH_0));
}
 
int32_t hardware_pwm_set_sense_on_toff_before_ton_us(int32_t time_us)
{
	pwm_time_before_ton_start_us = time_us;
	return 0;
}

void    hardware_pwm_start_with_ton									(void)
{
	TM_SetCounter(HT_MCTM0,TM_GetCaptureCompare(HT_MCTM0,TM_CH_0));
}

void 		hardware_pwm_set_outputs_to_toff						(void)
{
	TM_SetCounter(HT_MCTM0,0);
}


/* Inicio Codigo Jose --------------------------------------------------------- */
//Alternativa 1: Cargo el contador con el valor de cuenta necesario para mantener
// el toff haciendo CNTR = PERIODO - TOFF y el reinicio se hace automatico

// TODO: ESTO NO ANDA BIEN. Por alguna razon el metodo para el TM_OM_PWM2 no
// funciona pero el metodo para el TM_OM_PWM1 funciona en el 2
void hardware_pwm_set_counter_to_toff (void)
{
	/*
	u32 toff, periodo;
	toff = TM_GetCaptureCompare(HT_MCTM0,TM_CH_0);
	periodo = hardware_pwm_get_period_us();
		
	TM_SetCounter(HT_MCTM0,periodo - toff);
	*/
	
	
	if ( (HT_MCTM0->CH0OCFR & 0x0007) == TM_OM_PWM1)				//Si esta en modo 1
	{
		//CCR contiene el periodo
		//CH0CCR contiene el valor a comparar para cambiar de estado
		HT_MCTM0->CNTR = (HT_MCTM0->CRR+1) - (HT_MCTM0->CH0CCR);
	}
	if ( (HT_MCTM0->CH0OCFR & 0x0007) == TM_OM_PWM2)				//Si esta en modo 2
	{
		HT_MCTM0->CNTR = (HT_MCTM0->CRR)-1;
		//HT_MCTM0->CNTR = 0;
	}
	//TM_GenerateEvent(HT_MCTM0, TM_EVENT_BRKEV);
}


//Alternativa 2: Fuerzo el bit de reset del contador usando el registro EVGR
// Este bit fuerza el reset del CNTR y se vuelve a 0 por hardware. 
// Ver paginas 180 y 224 del manual, ver tambien seccion reset
void 		hardware_pwm_reset_counter						(void)
{
	TM_GenerateEvent(HT_MCTM0, TM_EVENT_BRKEV);		//Revisar el primer argumento de esta funcion, puede estar mal
}


/* Fin Codigo Jose ------------------------------------------------------------ */

int32_t hardware_pwm_get_ton_elapsed								(void)
{
	int32_t value;
	value = TM_GetCounter(HT_MCTM0);
	if(value>TM_GetCaptureCompare(HT_MCTM0,TM_CH_0))
	{
		return (value-TM_GetCaptureCompare(HT_MCTM0,TM_CH_0));
	}
	else
		return 0;
}




static void micro_adc_config(void)
{

  /* ADCLK frequency is set to CK_AHB/64                                                                    */
  CKCU_SetADCPrescaler(CKCU_ADCPRE_DIV64);	
  /* Config AFIO mode as ADC function                                                                       */
  AFIO_GPxConfig(GPIO_PB, AFIO_PIN_7, AFIO_FUN_ADC);	
  AFIO_GPxConfig(GPIO_PB, AFIO_PIN_8, AFIO_FUN_ADC);	
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_1, AFIO_FUN_ADC);	
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_2, AFIO_FUN_ADC);	
	
	
  /* Continuous Mode, Length 1, SubLength 1                                                                 */
  ADC_RegularGroupConfig(HT_ADC, CONTINUOUS_MODE, 4, 1);	
	  /* ADC Channel n, Rank 0, Sampling clock is (1.5 + 0) ADCLK
     Conversion time = (sampling clock + 12.5) / ADCLK = 12.4 uS */
	ADC_SamplingTimeConfig(HT_ADC,10);
	
  ADC_RegularChannelConfig(HT_ADC, ADC_CH_0, 0);	//PB7: SHUNT_AVG
	ADC_RegularChannelConfig(HT_ADC, ADC_CH_1, 1);	//PB8: VBUS
	ADC_RegularChannelConfig(HT_ADC, ADC_CH_3, 2);	//PA1: NTC_PWR
	ADC_RegularChannelConfig(HT_ADC, ADC_CH_4, 3);	//PA2: NTC_HEATER
	
	  /* Use Software Trigger as ADC trigger source                                                             */
  ADC_RegularTrigConfig(HT_ADC, ADC_TRIG_SOFTWARE);
	
	ADC_Cmd(HT_ADC, ENABLE);

  /* Software trigger to start continuous mode                                                              */
  ADC_SoftwareStartConvCmd(HT_ADC, ENABLE);
}







uint8_t hardware_uart_read (void)
{
	uint8_t	retval;
	
	
	if(RETARGET_USART_PORT->SR & (1<<5))	//check RXDY flag
	{
		retval = RETARGET_USART_PORT->DR;
		RETARGET_USART_PORT->SR|=(1<<5);		//clear RXDY flag
	}
	else
		retval = 0;
	
	return retval;
}

//void hardware_uart_transmit(char * fmt,...)
//{
	//https://stackoverflow.com/questions/1716296/why-does-printf-not-flush-after-the-call-unless-a-newline-is-in-the-format-strin
	//https://stackoverflow.com/questions/21540778/pass-varargs-to-printf
	//http://software-dl.ti.com/ccs/esd/documents/sdto_cgt_tips_for_using_printf.html

	//Esto es para habilitar impresion de Floats
	//http://www.openstm32.org/forumthread954
	//   va_list ap; // points to each unnamed arg in turn
	 //  va_start(ap, fmt); // make ap point to 1st unnamed arg
	  // vprintf(fmt,ap);
	  // fflush(stdout);

//	   va_end(ap); // clean up when done
//}

//-------------------------------------------
//				 TIMERS
//-------------------------------------------
static void micro_timers_config (void)
{
	TM_TimeBaseInitTypeDef TM_Struct_init;
	
	BFTM_ClearFlag(HT_BFTM0);
	BFTM_ClearFlag(HT_BFTM1);
	
	NVIC_EnableIRQ(BFTM0_IRQn);
	NVIC_EnableIRQ(BFTM1_IRQn);
  BFTM_SetCounter(HT_BFTM0, 0);
	BFTM_SetCounter(HT_BFTM1, 0);
  BFTM_IntConfig(HT_BFTM0, ENABLE);
	BFTM_IntConfig(HT_BFTM1, ENABLE);
	BFTM_OneShotModeCmd(HT_BFTM0,ENABLE);
	BFTM_OneShotModeCmd(HT_BFTM1,ENABLE);	
	
	TM_Struct_init.CounterMode = TM_CNT_MODE_UP;
	TM_Struct_init.CounterReload = 0xFFFF;
	TM_Struct_init.Prescaler = 39;
	TM_Struct_init.PSCReloadTime = TM_PSC_RLD_IMMEDIATE;
	TM_Struct_init.RepetitionCounter=0;
	
	TM_TimeBaseInit(HT_SCTM0,&TM_Struct_init);
	//TM_TimeBaseInit(HT_SCTM1,&TM_Struct_init);
	
	TM_ClearIntPendingBit(HT_SCTM0,TM_INT_CH0CC);
	//TM_ClearIntPendingBit(HT_SCTM1,TM_INT_CH0CC);
	TM_ClearFlag(HT_SCTM0,TM_INT_UEV);
	//TM_ClearFlag(HT_SCTM1,TM_INT_UEV);
	
	NVIC_EnableIRQ(SCTM0_IRQn);
	//NVIC_EnableIRQ(SCTM1_IRQn);
	TM_SetCounter(HT_SCTM0,0);
	//TM_SetCounter(HT_SCTM1,0);
	TM_IntConfig(HT_SCTM0,TM_INT_UEV,ENABLE);	
	//TM_IntConfig(HT_SCTM1,TM_INT_UEV,ENABLE);
}


void SCTM0_IRQHandler (void)
{
	TM_Cmd(HT_SCTM0, DISABLE);					//Resuelta la IRQ apago el timer hasta la proxima configuracion
	TM_IntConfig(HT_SCTM0,TM_INT_UEV,DISABLE);
	TM_ClearFlag(HT_SCTM0,TM_INT_UEV);
	TM_SetCounter(HT_SCTM0,0);

	(*func_ptr_callback_sctm0)();	
}

void SCTM1_IRQHandler (void)
{
	TM_ClearFlag(HT_SCTM1,TM_INT_UEV);
	(*func_ptr_callback_sctm1)();
}



void micro_config (void)
{
	//RECORDAR DE MODIFICAR EL ARCHIVO "system_ht32f5xxxxx_02.c" PARA CONFIGURAR EL CLOCK CON EL OSC INTERNO (HSI)
		
  	NVIC_Configuration();               /* NVIC configuration                                                 */
  	CKCU_Configuration();               /* System Related configuration                                       */
  	GPIO_Configuration();               /* GPIO Related configuration                                         */
  	RETARGET_Configuration();           /* Retarget Related configuration                                     */
	SYSTICK_Configuration();
	
	micro_gpios_outputs_pp_config();	
	micro_gpios_inputs_pullup_config();
	micro_gpios_hiz_config();
	micro_gpios_edge_events_config();
	
	
	micro_pwm_config();
	
	micro_timers_config();

	micro_adc_config();	
}
	



/*********************************************************************************************************//**
 * @brief   This function handles BFTM0 interrupt.
 * @retval  None
 ************************************************************************************************************/
void BFTM0_IRQHandler(void)
{
  BFTM_ClearFlag(HT_BFTM0);
	 (*func_ptr_callback_bftm0)();
}
/*********************************************************************************************************//**
 * @brief   This function handles BFTM1 interrupt.
 * @retval  None
 ************************************************************************************************************/
void BFTM1_IRQHandler(void)
{
  BFTM_ClearFlag(HT_BFTM1);
	(*func_ptr_callback_bftm1)();
}

int32_t hardware_htim_link_callback (void (*func_pointer)(void), uint32_t __hardware_tim_sel)
{
	switch(__hardware_tim_sel)
	{
		case __HARDWARE_TIM_BFTM0_SEL: func_ptr_callback_bftm0 = func_pointer;
															break;
		case __HARDWARE_TIM_BFTM1_SEL: func_ptr_callback_bftm1 = func_pointer;
															break;		
		case __HARDWARE_TIM_SCTM0_SEL: func_ptr_callback_sctm0 = func_pointer;
															break;
		case __HARDWARE_TIM_SCTM1_SEL: func_ptr_callback_sctm1 = func_pointer;
															break;
	
		default:	return -1;
	}
	
	return 0;
}



#define EXTI_CALLBACKS_MACRO(EXTI_CHANNEL)	{if(EXTI_GetEdgeStatus(EXTI_CHANNEL,EXTI_EDGE_POSITIVE)){\
																							(*func_ptr_callback_rising_exti[EXTI_CHANNEL])();EXTI_ClearEdgeFlag(EXTI_CHANNEL);}\
																						 if(EXTI_GetEdgeStatus(EXTI_CHANNEL,EXTI_EDGE_NEGATIVE)){\
																							(*func_ptr_callback_falling_exti[EXTI_CHANNEL])();EXTI_ClearEdgeFlag(EXTI_CHANNEL);}}
										
void EXTI0_1_IRQHandler(void)
{
	EXTI_CALLBACKS_MACRO(EXTI_CHANNEL_0);
	EXTI_CALLBACKS_MACRO(EXTI_CHANNEL_1);
}

void EXTI2_3_IRQHandler(void)
{
	EXTI_CALLBACKS_MACRO(EXTI_CHANNEL_2);
	EXTI_CALLBACKS_MACRO(EXTI_CHANNEL_3);
}

void EXTI4_15_IRQHandler(void)
{
	EXTI_CALLBACKS_MACRO(EXTI_CHANNEL_4);
	EXTI_CALLBACKS_MACRO(EXTI_CHANNEL_5);
	EXTI_CALLBACKS_MACRO(EXTI_CHANNEL_6);
	EXTI_CALLBACKS_MACRO(EXTI_CHANNEL_7);
	EXTI_CALLBACKS_MACRO(EXTI_CHANNEL_8);
	EXTI_CALLBACKS_MACRO(EXTI_CHANNEL_9);
	EXTI_CALLBACKS_MACRO(EXTI_CHANNEL_10);
	EXTI_CALLBACKS_MACRO(EXTI_CHANNEL_11);
	EXTI_CALLBACKS_MACRO(EXTI_CHANNEL_12);
	EXTI_CALLBACKS_MACRO(EXTI_CHANNEL_13);
	EXTI_CALLBACKS_MACRO(EXTI_CHANNEL_14);
	EXTI_CALLBACKS_MACRO(EXTI_CHANNEL_15);
}

//Linkea la funcion que se desea llamar cuando suceda un evento EXTI (tipo rising)
//Argumentos: - puntero de la funcion que se desea llamar cuando haya un evento rising en la linea indicada por el otro argumento
//            - Linea a la cual se desea linkear el callback (de 0 a 15)
int32_t hardware_gpio_event_rising_event_link_callback(void (*func_pointer)(void),uint32_t exti_line)
{ 
	if(exti_line<=15)
		func_ptr_callback_rising_exti[exti_line] = func_pointer;
	else
		return -1;
	
	return 0;
}
//Linkea la funcion que se desea llamar cuando suceda un evento EXTI (tipo falling)
//Argumentos: - puntero de la funcion que se desea llamar cuando haya un evento rising en la linea indicada por el otro argumento
//            - Linea a la cual se desea linkear el callback (de 0 a 15)
int32_t hardware_gpio_event_falling_event_link_callback(void (*func_pointer)(void),uint32_t exti_line)
{
	if(exti_line<=15)
		func_ptr_callback_falling_exti[exti_line] = func_pointer;
	else
		return -1;
	
	return 0;
}



/*********************************************************************************************************//**
 * Funcion Creada por Jose
 * @brief Enable or Disable the TMx channel N or not N.
 * @param TMx: where TMx is the selected TM from the TM peripheral.
 * @param Channel: Specify the TM channel.
 *        This parameter can be one of the following values:
 *        @arg TM_CH_0 : TM channel 0
 *        @arg TM_CH_1 : TM channel 1
 *        @arg TM_CH_2 : TM channel 2
 *        @arg TM_CH_3 : TM channel 3
 * @param Control: This parameter can be TM_CHCTL_ENABLE or TM_CHCTL_DISABLE.
 * @param Type: Type of Channel, can be NORMAL or COMPLEMENTARY
 * @retval None
 ************************************************************************************************************/
void TM_ChannelED(HT_TM_TypeDef* TMx, TM_CH_Enum Channel, TM_CHCTL_Enum Control, int Type)
{
  /* Check the parameters                                                                                   */
  Assert_Param(IS_TM(TMx));
  Assert_Param(IS_TM_CH(Channel));
  Assert_Param(IS_TM_CHCTL(Control));
	
	/*
	if(Channel & 0x03)					// TM_CH_3
	{
		Type = 0;
		Channel = 0x04;
	}
	*/
	
	// Reset the CHxE Bit
	TMx->CHCTR &= ~(u32)((0x01 << (Channel*2)) << Type);

	// Set the CHxE Bit if aplies
	TMx->CHCTR |= (u32)((Control << (Channel*2)) << Type);
}



/*********************************************************************************************************//**
  * @brief  Configure the NVIC Systick
  * @retval None
  ***********************************************************************************************************/
void SYSTICK_Configuration(void)
{
	 /* SYSTICK configuration */
  SYSTICK_ClockSourceConfig(SYSTICK_SRC_STCLK);       // Default : CK_SYS/8
  SYSTICK_SetReloadValue(SystemCoreClock / 8 / 1000); // (CK_SYS/8/1000) = 1ms on chip
  SYSTICK_IntConfig(ENABLE);                          // Enable SYSTICK Interrupt
	SYSTICK_CounterCmd(SYSTICK_COUNTER_CLEAR);
  SYSTICK_CounterCmd(SYSTICK_COUNTER_ENABLE);
}

	
/*********************************************************************************************************//**
  * @brief  Configure the NVIC vector table.
  * @retval None
  ***********************************************************************************************************/
void NVIC_Configuration(void)
{
  NVIC_SetVectorTable(NVIC_VECTTABLE_FLASH, 0x0);     /* Set the Vector Table base location at 0x00000000   */
}

/*********************************************************************************************************//**
  * @brief  Configure the system clocks.
  * @retval None
  ***********************************************************************************************************/
void CKCU_Configuration(void)
{
/*
//<e0> Enable Peripheral Clock
//  <h> Communication
//    <q5> EBI
//    <q11> I2C0   <q12> I2C1
//    <q23> I2S
//    <q21> SCI0 <q22> SCI1
//    <q13> SPI0   <q14> SPI1
//    <q17> UART0  <q18> UART1
//    <q15> USART0 <q16> USART1
//    <q3>  USB
//  </h>
//  <h> IO
//    <q7> GPIO Port A <q8>  GPIO Port B <q9>  GPIO Port C <q10>  GPIO Port D
//    <q19> AFIO
//    <q20> EXTI
//  </h>
//  <h> System
//    <q32> ADC
//    <q4>  CKREF
//    <q6>  CRC
//    <q31> CMP
//    <q2>  PDMA
//    <q26> PWRCU
//  </h>
//  <h> Timer
//    <q29> BFTM0 <q30> BFTM1
//    <q33> SCTM0 <q34> SCTM1 <q35> SCTM2 <q36> SCTM3
//    <q27> GPTM0 <q28> GPTM1
//    <q24> MCTM0
//    <q26> RTC   <q25> WDT
//  </h>
//</e>
*/
#if 1
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{ 0 }};
  CKCUClock.Bit.PDMA       = 0;
  CKCUClock.Bit.USBD       = 0;
  CKCUClock.Bit.CKREF      = 0;
  CKCUClock.Bit.EBI        = 0;
  CKCUClock.Bit.CRC        = 0;
  CKCUClock.Bit.PA         = 0;
  CKCUClock.Bit.PB         = 0;
  CKCUClock.Bit.PC         = 0;
  CKCUClock.Bit.PD         = 0;
  CKCUClock.Bit.I2C0       = 0;
  CKCUClock.Bit.I2C1       = 0;
  CKCUClock.Bit.SPI0       = 0;
  CKCUClock.Bit.SPI1       = 0;
  CKCUClock.Bit.USART0     = 1;
  CKCUClock.Bit.USART1     = 1;
  CKCUClock.Bit.UART0      = 1;
  CKCUClock.Bit.UART1      = 1;
  CKCUClock.Bit.AFIO       = 1;
  CKCUClock.Bit.EXTI       = 1;
  CKCUClock.Bit.SCI0       = 0;
  CKCUClock.Bit.SCI1       = 0;
  CKCUClock.Bit.I2S        = 0;
  CKCUClock.Bit.MCTM0      = 1;
  CKCUClock.Bit.WDT        = 0;
  CKCUClock.Bit.BKP        = 0;
  CKCUClock.Bit.GPTM0      = 0;
  CKCUClock.Bit.GPTM1      = 0;
  CKCUClock.Bit.BFTM0      = 1;
  CKCUClock.Bit.BFTM1      = 1;
  CKCUClock.Bit.CMP        = 0;
  CKCUClock.Bit.ADC        = 1;
  CKCUClock.Bit.SCTM0      = 1;
  CKCUClock.Bit.SCTM1      = 1;
  CKCUClock.Bit.SCTM2      = 0;
  CKCUClock.Bit.SCTM3      = 0;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
#endif

#if (ENABLE_CKOUT == 1)
  CKOUTConfig();
#endif
}

#if (ENABLE_CKOUT == 1)
/*********************************************************************************************************//**
  * @brief  Configure the debug output clock.
  * @retval None
  ***********************************************************************************************************/
void CKOUTConfig(void)
{
  CKCU_CKOUTInitTypeDef CKOUTInit;

  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_9, AFIO_MODE_15);
  CKOUTInit.CKOUTSRC = CKCU_CKOUTSRC_HCLK_DIV16;
  CKCU_CKOUTConfig(&CKOUTInit);
}
#endif

/*********************************************************************************************************//**
  * @brief  Configure the GPIO ports.
  * @retval None
  ***********************************************************************************************************/
void GPIO_Configuration(void)
{
#if (RETARGET_PORT == RETARGET_USART0)
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_2 | AFIO_PIN_3, AFIO_FUN_USART_UART);
#endif

#if (RETARGET_PORT == RETARGET_USART1)
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_4 | AFIO_PIN_5, AFIO_FUN_USART_UART);
#endif

#if (RETARGET_PORT == RETARGET_UART0)
  AFIO_GPxConfig(GPIO_PC, AFIO_PIN_4 | AFIO_PIN_5, AFIO_FUN_USART_UART);
#endif

#if (RETARGET_PORT == RETARGET_UART1)
  AFIO_GPxConfig(GPIO_PC, AFIO_PIN_1 | AFIO_PIN_3, AFIO_FUN_USART_UART);
#endif
}

#if (HT32_LIB_DEBUG == 1)
/*********************************************************************************************************//**
  * @brief  Report both the error name of the source file and the source line number.
  * @param  filename: pointer to the source file name.
  * @param  uline: error line source number.
  * @retval None
  ***********************************************************************************************************/
void assert_error(u8* filename, u32 uline)
{
  /*
     This function is called by IP library that the invalid parameters has been passed to the library API.
     Debug message can be added here.
     Example: printf("Parameter Error: file %s on line %d\r\n", filename, uline);
  */

  while (1)
  {
  }
}
#endif

/* Private functions ---------------------------------------------------------------------------------------*/



/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
