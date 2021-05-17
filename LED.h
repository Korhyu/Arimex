/*********************************************************************************************************//**
 * @file    LED.h
 * @version $Rev:: 1040         $
 * @date    $Date:: 2019-06-10 #$
 * @brief   The header file of LED related functions.
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
/* Define to prevent recursive inclusion -------------------------------------------------------------------*/
#ifndef __LED_H
#define __LED_H

/* Settings ------------------------------------------------------------------------------------------------*/
#define LED_NUM                       (3)

/* Exported typedef ----------------------------------------------------------------------------------------*/
typedef enum
{
  LED1 = 0,
  LED2 = 1,
  LED3 = 2,
} LED_TypeDef;

/* Exported functions --------------------------------------------------------------------------------------*/
void LED_Init(void);
void LED_On(LED_TypeDef LED);
void LED_Off(LED_TypeDef LED);
void LED_Toggle(LED_TypeDef LED);

#endif
