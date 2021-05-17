/*********************************************************************************************************//**
 * @file    LED.c
 * @version $Rev:: 1206         $
 * @date    $Date:: 2020-01-31 #$
 * @brief   The LED related functions.
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
#include "LED.h"
#include "ht32_board_config.h"

/* Private variables ---------------------------------------------------------------------------------------*/
static HT_GPIO_TypeDef* LEDPort[LED_NUM] =
{
  LED1_PORT,
  LED2_PORT,
  LED3_PORT,
};

static u32 uLEDPin[LED_NUM] =
{
  1UL << LED1_PIN,
  1UL << LED2_PIN,
  1UL << LED3_PIN,
};

/* Global functions ----------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
 * @brief Configure alternated mode of GPIO with specified pins.
 * @param GPIO_Px: 0: PA ~ 5: PF.
 * @param GPIO_PIN: 0 ~ 15
 * @retval None
 ************************************************************************************************************/
void GPIO_ModeConfig(u32 GPIO_Px, u32 GPIO_PIN)
{
  vu32* pGPxCFGR = ((vu32*)&HT_AFIO->GPACFGR[0]) + GPIO_Px * 2;
  u32 uRegTmp;

  if (GPIO_PIN > 7)
  {
    GPIO_PIN -= 8;
    pGPxCFGR++;
  }
  uRegTmp = *pGPxCFGR;
  uRegTmp &= ~(0xF << (GPIO_PIN * 4));
  uRegTmp |= (1 << (GPIO_PIN * 4));
  *pGPxCFGR = uRegTmp;
}

/*********************************************************************************************************//**
  * @brief  LED initialization.
  * @retval None
  ***********************************************************************************************************/
void LED_Init(void)
{
  HT_CKCU->APBCCR0 |=  (1UL << 14);            /* Enable AFIO Clock                                         */
  HT_CKCU->AHBCCR  |=  (0x003F0000);           /* Enable GPIO Port A ~ F Clock (bit 16 ~ 21)                */

  GPIO_ModeConfig(LED1_PORT_NUM, LED1_PIN);    /* Configure PXn as GPIO                                     */
  LED1_PORT->DIRCR |=  (1UL << LED1_PIN);      /* Configure as Output                                       */
  LED1_PORT->DOUTR |=  (1UL << LED1_PIN);      /* Switch LED off                                            */

  GPIO_ModeConfig(LED2_PORT_NUM, LED2_PIN);    /* Configure PXn as GPIO                                     */
  LED2_PORT->DIRCR |=  (1UL << LED2_PIN);      /* Configure as Output                                       */
  LED2_PORT->DOUTR |=  (1UL << LED2_PIN);      /* Switch LED off                                            */

  GPIO_ModeConfig(LED3_PORT_NUM, LED3_PIN);    /* Configure PXn as GPIO                                     */
  LED3_PORT->DIRCR |=  (1UL << LED3_PIN);      /* Configure as Output                                       */
  LED3_PORT->DOUTR |=  (1UL << LED3_PIN);      /* Switch LED off                                            */
}

/*********************************************************************************************************//**
  * @brief  Turn on LED.
  * @param  LED: LEDn
  * @retval None
  ***********************************************************************************************************/
void LED_On(LED_TypeDef LED)
{
  if (LED < LED_NUM)
  {
    LEDPort[LED]->DOUTR &= ~uLEDPin[LED];
  }
}

/*********************************************************************************************************//**
  * @brief  Turn off LED.
  * @param  LED: LEDn
  * @retval None
  ***********************************************************************************************************/
void LED_Off(LED_TypeDef LED)
{
  if (LED < LED_NUM)
  {
    LEDPort[LED]->DOUTR |= uLEDPin[LED];
  }
}

/*********************************************************************************************************//**
  * @brief  Toggle LED.
  * @param  LED: LEDn
  * @retval None
  ***********************************************************************************************************/
void LED_Toggle(LED_TypeDef LED)
{
  if (LED < LED_NUM)
  {
    LEDPort[LED]->DOUTR ^= uLEDPin[LED];
  }
}

/* Private functions ---------------------------------------------------------------------------------------*/
