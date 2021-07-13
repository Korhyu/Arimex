#include "hardware_functions_low_level.h"


TIM_HandleTypeDef htim1,htim2,htim3,htim15;
UART_HandleTypeDef huart1;
ADC_HandleTypeDef hadc;
DMA_HandleTypeDef hdma_adc;
COMP_HandleTypeDef hcomp1,hcomp2;

int32_t pwm_time_before_ton_start_us=10;


uint32_t is_the_value_within_the_band(uint32_t value,uint32_t center_value,uint32_t tolerance_in_2_pow)
{
	int32_t aux;
	
	if(value>center_value)
		aux=value-center_value;
	else
		aux=center_value-value;
	
	if(aux < (center_value>>tolerance_in_2_pow))
		return 1;
	else
		return 0;
}


// --------------------------------------------------------------------------
// Inicializacion de todos los perisfericos - 
// Aca se listan e indican los usos/conexiones de cada perisferico para 
// evitar errores de inicializacion o configuracion.
// Esta configuracion de hardware es compatible con PCB Trifase BLDC 3 TECH REV2019-00 y REV2019-01
// 
//                *********ATENCION***********
// SI SE MODIFICA EL CIRCUITO HAY QUE MODIFICAR ESTA FUNCION 
// --------------------------------------------------------------------------
void hardware_config (void)
{
	//Checkear este listado con los esquematicos del hardware utilizado
	//PCB Trifase BLDC 3 TECH REV2019-00 y REV2019-01
  //
	//Funcionalidades de la placa:
	//-Limite de corriente cycle-by-cycle. Conduce full el pwm hasta llegar al limite y apaga por un t_off determinado. 
	// Mediante comparador interno (y/o ADC con watchdog) y pwm interno.
	//
	//-Blanking de BEMF con hardware externo: señales BEMF_SIGN_SLOPE_SELECT_XX y BEMF_BLANKING_MODE
	// 
	//-Seleccion de comparacion de BEMF con VBUS/2 o centro de estrella mediante señal BEMF_REF_SELECT
	//
	//-Potenciometro externo para variar configuraciones segun soft
	//
	//-Sensado de tensiones para determinar velocidad a rotor libre - Tension de linea - temperatura PCB en 
	// componentes de potencia - Potenciometro - Corrientes de fases para limite de corriente pico - Mediante canales del ADC
	//
	//-Pines en high-Z o salidas tipo Open-Drain para multiplexar la señal del PWM "centered" generado por hw externo 
	// ATENCION: En la REV2019-00 no fue cableada la señal para apagar HIN1!, en la REV2019-01 fue corregido.
	//
	//-Deteccion de flancos (digital) para sensar hardware externo (Sensor HALL externo)
	//
	//-GPIOs salida para leds y GPIOs entrada para pulsadores
	//
	//-UART para debug por consola 
	//
	//Comparadores - COMP1: 	se usa para comparar señales de BEMF 					           - PA0  - PA1	-	PA4	-PA5	-PA11
	//												Hay un timer asociado con esta funcionalidad
	//												Ya que hay que generar "blankings" ya que apenas se conmuta
	//									  	  de secuencia, hay un pico de tension.
	//												PA0: U_SENSE
	//												PA4: V_SENSE
	//												PA5: W_SENSE
	//												PA1: BEMF_VREF
	//												PA11:OUT_COMP1
	//
	//
	//ADCs				 - AN2  : se usa para medir la suma de las 3 corrientes CURR_TOT	 - PA2
	//						 - AN3  : Potenciometro externo para variar parametros segun soft  - PA3
	//						 - AN6  : detector de velocidad (detector de envolvente de la señal- PA6 
	//											star_center de bemf)
	//						 - AN9  : NTC_SENSE para sensar temperatura de MOSFETS de potencia - PB1
	//						 - AN10 : se usa para medir corriente de la fase "U" o CURR1			 - PC0
	//						 - AN11 : se usa para medir corriente de la fase "V" o CURR2			 - PC1								
	//						 - AN12 : se usa para medir corriente de la fase "W" o CURR3			 - PC2
	//						 - AN13 : se usa para medir la tension VBUS												 - PC3							
	//					   
	//
	//GPIOs HIz/OD - Multiplexacion de las señales de PWM. HIN1 HIN2 y HIN3
	//						 - HIN1 : ATENCION: En REV2019-00 no esta cableado correctamente. En REV2019-01 es PC6
	//						 - HIN2 : PA9
	//						 - HIN3 : PA10
	//						
	//GPIOs / EdgeEvents  : se usan para detectar los flancos de Zero-Crossing			 - PC14
	//						 - Hardware externo , conector J4 conectado a PC14.
	//
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
	//
	//GPIO_INPUTS					: se usan para leer pulsadores 														 - PSW0 : PC11 (SLIDE SWITCH)
	//																																							 - PSW1 : PB9
	//																																							 - PSW2 : PC13
	//																																							 - PSW3 : PF7
	//																																							 - PSW4 : PF6
	//																																							 - PSW5 : PB8
	//																																							 - PSW6 : PC12
	//																																							
	//
	//USART1							: se usa como consola para debug													 - PB6  - PB7
	//
	//
	//PWM									: se usa el TIMER 1 para usar funcion de PWM							 - PA8
	//											
	//TIMERS							: TO_DO: Descripcion de que timers se van a usar y para que
	//
  //
	//
	//
	//
	//
	 /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
	pcb_hardware_adc_config();
	pcb_hardware_dma_config();

  pcb_hardware_gpios_outputs_pp_config();		//Listo - Probado
	pcb_hardware_gpios_inputs_config();
  pcb_hardware_gpios_hiz_config();
	hardware_gpios_edge_events_config();
	pcb_hardware_uart_config();
	pcb_hardware_pwm_config();
	pcb_hardware_comp_config();	
	hardware_tim2_config();
	hardware_tim3_config();
	hardware_tim15_config();
}




void hardware_gpios_edge_events_config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.Pin = GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);

	HAL_NVIC_SetPriority(EXTI4_15_IRQn,0,0);

__hardware_exti0_reset_falling_edge_irq();
__hardware_exti0_reset_rising_edge_irq();
__hardware_exti0_enable_irq();

	
}



/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}


static void pcb_hardware_comp_config (void)
{
	//I Peak Limiter
	hcomp2.Instance = COMP2;
  hcomp2.Init.InvertingInput = COMP_INVERTINGINPUT_IO1;
  hcomp2.Init.NonInvertingInput = COMP_NONINVERTINGINPUT_IO1;
  hcomp2.Init.Output = COMP_OUTPUT_NONE;
  hcomp2.Init.OutputPol = COMP_OUTPUTPOL_NONINVERTED;
  hcomp2.Init.Hysteresis = COMP_HYSTERESIS_HIGH;
  hcomp2.Init.Mode = COMP_MODE_LOWPOWER;
  hcomp2.Init.WindowMode = COMP_WINDOWMODE_DISABLE;
  hcomp2.Init.TriggerMode = COMP_TRIGGERMODE_IT_RISING;
  if (HAL_COMP_Init(&hcomp2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	
	hcomp1.Instance=COMP1;
	hcomp1.Init.NonInvertingInput=COMP_NONINVERTINGINPUT_IO1;
	hcomp1.Init.InvertingInput = COMP_INVERTINGINPUT_IO1;
	hcomp1.Init.Output = COMP_OUTPUT_NONE;
	hcomp1.Init.OutputPol = COMP_OUTPUTPOL_NONINVERTED;
	hcomp1.Init.Hysteresis=COMP_HYSTERESIS_HIGH;
	hcomp1.Init.Mode = COMP_MODE_HIGHSPEED;
	hcomp1.Init.WindowMode = COMP_WINDOWMODE_DISABLE;
	hcomp1.Init.TriggerMode = COMP_TRIGGERMODE_IT_FALLING;
  if (HAL_COMP_Init(&hcomp1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

	
	HAL_NVIC_SetPriority(ADC1_COMP_IRQn,0,0);

__hardware_comp1_reset_falling_edge_irq();
__hardware_comp1_reset_rising_edge_irq();	
__HAL_COMP_COMP1_EXTI_CLEAR_FLAG();
__HAL_COMP_COMP2_EXTI_CLEAR_FLAG();
	
	HAL_NVIC_EnableIRQ(ADC1_COMP_IRQn);
	
	HAL_COMP_Start_IT(&hcomp1);
	HAL_COMP_Start_IT(&hcomp2);
__HAL_COMP_COMP1_EXTI_CLEAR_FLAG();
__HAL_COMP_COMP2_EXTI_CLEAR_FLAG();
	__hardware_comp2_set_falling_edge_irq_only();
	
	__hardware_comp_bemf_set_falling_edge_irq_only();

}


static void pcb_hardware_gpios_hiz_config(void)
{
	//GPIOs HIz/OD - Multiplexacion de las señales de PWM. HIN1 HIN2 y HIN3
	//						 - HIN1 : ATENCION: En REV2019-00 no esta cableado correctamente. Se usa PB9. En REV2019-01 es PC6
	//						 - HIN2 : PA9
	//						 - HIN3 : PA10
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
	GPIO_InitStruct.Mode= GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull= GPIO_NOPULL;
	GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10,GPIO_PIN_RESET);

	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode= GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull= GPIO_NOPULL;
	GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_RESET);	
	

	//ESTO DE ABAJO HAY QUE SACARLO EN LA VERSION NUEVA DE HW (A PARTIR DE LA 1 EN ADELANTE)
	
	GPIO_InitStruct.Pin = GPIO_PIN_9;
	GPIO_InitStruct.Mode= GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull= GPIO_NOPULL;
	GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_RESET);	
	
	GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_12;
	GPIO_InitStruct.Mode= GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull= GPIO_NOPULL;
	GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);	
	
	GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_6;
	GPIO_InitStruct.Mode= GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull= GPIO_NOPULL;
	GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_7,GPIO_PIN_RESET);	
	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_6,GPIO_PIN_SET);	
}

static void pcb_hardware_gpios_inputs_config(void)
{
	//GPIO_INPUTS					: se usan para leer pulsadores 														 - PSW0 : PC11 (SLIDE SWITCH)
	//																																							 - PSW1 : PB9
	//																																							 - PSW2 : PC13
	//																																							 - PSW3 : PF7
	//																																							 - PSW4 : PF6
	//																																							 - PSW5 : PB8
	//																																							 - PSW6 : PC12
		GPIO_InitTypeDef GPIO_InitStruct;
	
	
	//En la primer version de HW se usa PB9 para multiplexar HIN1 (HiZ)
		GPIO_InitStruct.Pin = GPIO_PIN_8;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

		HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);
	
		GPIO_InitStruct.Pin = GPIO_PIN_11/*|GPIO_PIN_12*/;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOC,&GPIO_InitStruct);
	
		GPIO_InitStruct.Pin = GPIO_PIN_6;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOF,&GPIO_InitStruct);
}

static void pcb_hardware_gpios_outputs_pp_config(void)
{
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
	
	GPIO_InitTypeDef GPIO_InitStruct;
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_15|GPIO_PIN_13|GPIO_PIN_12|GPIO_PIN_4|GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD,&GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOF,&GPIO_InitStruct);	
}



uint32_t hardware_pwm_get_ton(void)
{
	return (__HAL_TIM_GET_AUTORELOAD(&htim1)-__HAL_TIM_GET_COMPARE(&htim1,TIM_CHANNEL_1)+1);
}


void hardware_pwm_set_time_for_sense_zcd_before_ton_starts_us(uint32_t time_us)
{
	pwm_time_before_ton_start_us = time_us;
}

void hardware_pwm_set_period_us(uint32_t period_us)
{
	__hardware_pwm_set_period_us(period_us);
}
void hardware_pwm_set_ton_us(uint32_t ton_us)
{
	__hardware_pwm_set_ton_us(ton_us);
}

void hardware_pwm_set_toff_us(uint32_t toff_us)
{
	__hardware_pwm_set_toff_us(toff_us);
}

void hardware_pwm_set_outputs_to_toff(void)
{
	__hardware_pwm_set_outputs_to_toff();
}

uint32_t hardware_pwm_get_ton_elapsed(void)
{
	return __hardware_pwm_get_ton_elapsed();
}

void hardware_pwm_start_from_0(void)
{
	__hardware_pwm_start_from_0();
}

void hardware_pwm_reset_counter(void)
{
	__hardware_pwm_reset_counter();
	
}

void hardware_tim2_config(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig;
	TIM_MasterConfigTypeDef sMasterConfig;
	
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 23; //cuentas de 500ns
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 0xFFFF;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}


void hardware_tim3_config(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig;
	TIM_MasterConfigTypeDef sMasterConfig;
	
		//Timer 3: ZCD Y PERIODO  - tick de 500nS con IRQ
	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 23; //cuentas de 500ns
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 0xFFFF;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}


void hardware_tim15_config(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig;
	TIM_MasterConfigTypeDef sMasterConfig;
	
		//Timer 15: ZCD Y PERIODO  - tick de 500nS con IRQ
	htim15.Instance = TIM15;
	htim15.Init.Prescaler = 23; //cuentas de 500ns
	htim15.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim15.Init.Period = 0xFFFF;
	htim15.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim15.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim15) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim15, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim15, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}




/* TIM1 init function */
static void pcb_hardware_pwm_config(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;
  GPIO_InitTypeDef GPIO_InitStruct;
	
	
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 47;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 10;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

	sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	
	//-------------------------------
	//Config TIM_CH1
	//-------------------------------
  sConfigOC.OCMode = TIM_OCMODE_PWM2;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }


  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//-------------------------------
	//Config TIM_CH2
	//-------------------------------
  sConfigOC.OCMode = TIM_OCMODE_PWM2;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	
	
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);

	HAL_TIMEx_PWMN_Start(&htim1,TIM_CHANNEL_2);
	
	__hardware_pwm_hin1_reset();
	__hardware_pwm_hin2_reset();
	__hardware_pwm_hin3_reset();
	__hardware_pwm_lin1_reset();
	__hardware_pwm_lin2_reset();
	__hardware_pwm_lin3_reset();
}




//----------------------------------------------------------
//---------------------ADC CON DMA--------------------------
//----------------------------------------------------------

static volatile uint32_t adc_valores[ADC_CHANNEL_COUNT];		//Buffer DMA del ADC
static volatile uint32_t buffer[ADC_CHANNEL_COUNT];

volatile uint32_t * hardware_adc_get_data_p(void)
{
	return (&adc_valores[0]);
}

int32_t hardware_adc_get_channels_count(void)
{
	return ADC_CHANNEL_COUNT;
}


void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */

  /* USER CODE END DMA1_Channel1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc);
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */

  /* USER CODE END DMA1_Channel1_IRQn 1 */
}




void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	for(int i=0;i<ADC_CHANNEL_COUNT;i++)
	{
			adc_valores[i] = buffer[i];
	}
	
}

static void pcb_hardware_dma_config(void)
{
	  /* DMA controller clock enable */
  //__HAL_RCC_DMA1_CLK_ENABLE();
  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  //HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  //HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	//El codigo de arriba es para inicializar el ADC y que haga IRQ por cada EOT del DMA.

	/* (1) Enable the peripheral clock on DMA */
	/* (2) Enable DMA transfer on ADC and circular mode */
	/* (3) Configure the peripheral data register address */
	/* (4) Configure the memory address */
	/* (5) Configure the number of DMA tranfer to be performs on DMA channel 1 */
	/* (6) Configure increment, size, interrupts and circular mode */
	/* (7) Enable DMA Channel 1 */
	RCC->AHBENR |= RCC_AHBENR_DMA1EN; /* (1) */
	DMA1_Channel1->CPAR = (uint32_t) (&(ADC1->DR)); /* (3) */
	DMA1_Channel1->CMAR = (uint32_t)(adc_valores); /* (4) */
	DMA1_Channel1->CNDTR = ADC_CHANNEL_COUNT; /* (5) */
	DMA1_Channel1->CCR |= DMA_CCR_MINC | DMA_CCR_MSIZE_1 | DMA_CCR_PSIZE_1 | DMA_CCR_CIRC; /* (6) */
										/*Memory Inc mode     MEM 32bits       PERIP 32bits       circular          */

	ADC1->CFGR1 |= ADC_CFGR1_DMAEN | ADC_CFGR1_DMACFG; /* (2) */
	DMA1_Channel1->CCR |= DMA_CCR_EN; /* (7) */
	
}

static void pcb_hardware_adc_config(void)
{
	//ADCs				 - AN2  : se usa para medir la suma de las 3 corrientes CURR_TOT	 - PA2
	//						 - AN6  : detector de velocidad (detector de envolvente de la señal- PA6 
	//											star_center de bemf)
	//						 - AN9  : NTC_SENSE para sensar temperatura de MOSFETS de potencia - PB1
	//						 - AN13 : se usa para medir la tension VBUS												 - PC3							
	
  ADC_ChannelConfTypeDef sConfig;

    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
    */
  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  hadc.Init.ContinuousConvMode = ENABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = ENABLE;
  hadc.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure for the selected ADC regular channel to be converted. 
    */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }


  sConfig.Channel = ADC_CHANNEL_6;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sConfig.Channel = ADC_CHANNEL_9;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

	sConfig.Channel = ADC_CHANNEL_13;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	
	HAL_ADCEx_Calibration_Start(&hadc);
	//HAL_ADC_Start(&hadc);
	HAL_ADC_Start_DMA(&hadc,buffer,ADC_CHANNEL_COUNT);
}




static void pcb_hardware_uart_config(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 38400;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }		
	
}


uint8_t hardware_uart_read (void)
{
	uint8_t var,retval;
	retval = HAL_UART_Receive(&huart1,&var,1,1);
	HAL_UART_Abort(&huart1);
	
	if(retval==HAL_OK)
	{
		return var;	
	}
	else
	{
		return retval;	
	}
}






// Esta funcion cambia el estado de los atenuadores de BEMF 
// en estado Enable, el atenuador permanece activado  y sirve para sensar ZCD BEMF en TON  del PWM
// en estado Disable,el atenuador permanece desactiva y sirve para sensar ZCD BEMF en TOFF del PWM
// en estado Auto, el atenuador se activa en TON y se desactiva en TOFF.
// La funcion devuelve resultado distinto de 0 si el parametro pasado está fuera de rango. Por defecto se desactiva la atenuacion.
uint32_t hardware_bemf_attenuation_select(uint32_t __hardware_bemf_attenuation_mode)
{
	switch(__hardware_bemf_attenuation_mode)
	{
		case __HARDWARE_BEMF_ATT_ENABLE:	__hardware_bemf_attenuation_enable();
																				break;
		case __HARDWARE_BEMF_ATT_DISABLE:	__hardware_bemf_attenuation_disable();
																				break;	
		case __HARDWARE_BEMF_ATT_AUTO:		__hardware_bemf_attenuation_auto();	
																				break;
		default:													__hardware_bemf_attenuation_disable();
																				return 1;
	}
	return 0;
}





#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE  
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 100);
    return ch;
}	



/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 
