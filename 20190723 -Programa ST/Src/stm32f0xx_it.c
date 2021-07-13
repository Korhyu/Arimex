/**
  ******************************************************************************
  * @file    stm32f0xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include "stm32f0xx.h"
#include "stm32f0xx_it.h"

/* USER CODE BEGIN 0 */

#include "hardware_functions_low_level.h"
/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles Non maskable interrupt.
*/
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
* @brief This function handles Hard fault interrupt.
*/
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
  }
  /* USER CODE BEGIN HardFault_IRQn 1 */

  /* USER CODE END HardFault_IRQn 1 */
}

/**
* @brief This function handles System service call via SWI instruction.
*/
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVC_IRQn 0 */

  /* USER CODE END SVC_IRQn 0 */
  /* USER CODE BEGIN SVC_IRQn 1 */

  /* USER CODE END SVC_IRQn 1 */
}

/**
* @brief This function handles Pendable request for system service.
*/
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

void ADC1_COMP_IRQHandler  (void)
{
	//Comparador de Bemf 
	if(__HAL_COMP_COMP1_EXTI_GET_FLAG()) 	
	{			
		//Por alguna razon cuando deshabilito y habilito el comparador salta a la IRQ, por eso hago estos checkeos		
		if(COMP->CSR&(1<<0) && (((EXTI->FTSR&COMP_EXTI_LINE_COMP1)!=0) || ((EXTI->RTSR&COMP_EXTI_LINE_COMP1)!=0)))
		{				
			hardware_comp_bemf_callback();
		}
		__HAL_COMP_COMP1_EXTI_CLEAR_FLAG();
	}
	
	//Comparador de Corriente
	else if(__HAL_COMP_COMP2_EXTI_GET_FLAG()) 
	{
		if(COMP->CSR&(1<<16) && (((EXTI->FTSR&COMP_EXTI_LINE_COMP2)!=0) || ((EXTI->RTSR&COMP_EXTI_LINE_COMP2)!=0)))//Por alguna razon cuando deshabilito el comparador salta a la IRQ, por eso hago este checkeo
		{
			hardware_comp_i_peak_callback();
		}
		
		__HAL_COMP_COMP2_EXTI_CLEAR_FLAG();
	}
	//else
	//	HAL_ADC_IRQHandler(&hadc1); //!!! ATENCION: Aca tal vez convenga escribir un handler propio para mayor velocidad
}



//----------------------------------------------------					
//Timer 2: ZCD BEMF BLANKING -  tick de 500nS	con IRQ
//----------------------------------------------------	
void TIM2_IRQHandler(void)
{
	__HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);
	HAL_TIM_Base_Stop_IT(&htim2);
	hardware_tim_blanking_callback();
}


void TIM3_IRQHandler(void)
{
	__HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE);
	HAL_TIM_Base_Stop_IT(&htim3);
	hardware_tim3_comm_callback();	
}

void TIM15_IRQHandler(void)
{
	__HAL_TIM_CLEAR_IT(&htim15, TIM_IT_UPDATE);
	HAL_TIM_Base_Stop_IT(&htim15);
	hardware_tim15_watchdog_callback();	
}



void EXTI0_1_IRQHandler (void) 
{
	//PWM EDGES
	if(__HAL_GPIO_EXTI_GET_FLAG(GPIO_PIN_0) )
	{
			if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_0) == GPIO_PIN_SET)
					exti0_rising_edge_callback();
			else
					exti0_falling_edge_callback();
			
			__HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_0);
	}

}


void EXTI4_15_IRQHandler(void)
{
	if(__HAL_GPIO_EXTI_GET_FLAG(GPIO_PIN_15) )
	{
			if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15) == GPIO_PIN_SET)
					exti0_rising_edge_callback();
			else
					exti0_falling_edge_callback();
			
			__HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_15);
	}	
}



/******************************************************************************/
/* STM32F0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f0xx.s).                    */
/******************************************************************************/

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
