/*********************************************************************************************************//**
 * @file    main.c
 * @version $Rev:: 1040         $
 * @date    $Date:: 2019-06-10 #$
 * @brief   The LED Blinky example code.
 *************************************************************************************************************
 * @attention
 *
 * Firmware Disclaimer Information
 *
 * 1. The customer hereby acknowledges and agrees that the program technical documentation, including the
 *    code, which is supplied by Holtek Semiconductor Inc., (hereinafter referred to as "HOLTEK") is the
 *    proprietary and confidential intellectual property of HOLTEK, and is protected by copyright law and
 *    other intellectual property laws.
 *
 * 2. The customer hereby acknowledges and agrees that the program technical documentation, including the
 *    code, is confidential information belonging to HOLTEK, and must not be disclosed to any third parties
 *    other than HOLTEK and the customer.
 *
 * 3. The program technical documentation, including the code, is provided "as is" and for customer reference
 *    only. After delivery by HOLTEK, the customer shall use the program technical documentation, including
 *    the code, at their own risk. HOLTEK disclaims any expressed, implied or statutory warranties, including
 *    the warranties of merchantability, satisfactory quality and fitness for a particular purpose.
 *
 * <h2><center>Copyright (C) Holtek Semiconductor Inc. All rights reserved</center></h2>
 ************************************************************************************************************/

/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32f5xxxx_01.h"
#include "ht32_board_config.h"
#include "LED.h"

/* Global variables ----------------------------------------------------------------------------------------*/
u32 uTime = 9;
u32 uLEDTime = 0;
u32 cont = 0;

/* Private functions ---------------------------------------------------------------------------------------*/
int main(void)
{
  LED_Init();

  LED_On(LED1);
  LED_Off(LED2);
  LED_On(LED3);

  SysTick_Config(SystemCoreClock / 100);     /* Generate interrupt each 10 ms                               */

  while (1)
  {
    if (uLEDTime == 1)
    {
      uLEDTime = 0;
      LED_Toggle(LED1);
      LED_Toggle(LED2);
      LED_Toggle(LED3);
			
			cont++;
			
			if(cont >= 9)
			{
				uTime = uTime+5;
				cont = 0;
				
				if (uTime >= 59)
				{
					uTime = 9;
				}
			}
			
    }
  }
}

/*********************************************************************************************************//**
 * @brief   This function handles SysTick Handler.
 * @retval  None
 ************************************************************************************************************/
void SysTick_Handler(void)
{
  static u32 uTicks = 0;
  uTicks++;
  if (uTicks >= uTime)
  {
    uTicks = 0;
    uLEDTime = 1;
  }
}
