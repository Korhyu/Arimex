/*********************************************************************************************************//**
 * @file    ht32_board_config.h
 * @version $Rev:: 1040         $
 * @date    $Date:: 2019-06-10 #$
 * @brief   The header file of board configuration.
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
#ifndef __HT32_BOARD_CONFIG_H
#define __HT32_BOARD_CONFIG_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Settings ------------------------------------------------------------------------------------------------*/
#define LED1_PORT_NUM                 (2)         // GPIOA = 0, GPIOB = 1, GPIOC = 2, GPIOD = 3, GPIOE = 4
#define LED1_PORT                     (HT_GPIOC)
#define LED1_PIN                      (2)

#define LED2_PORT_NUM                 (2)         // GPIOA = 0, GPIOB = 1, GPIOC = 2, GPIOD = 3, GPIOE = 4
#define LED2_PORT                     (HT_GPIOC)
#define LED2_PIN                      (3)

#define LED3_PORT_NUM                 (1)         // GPIOA = 0, GPIOB = 1, GPIOC = 2, GPIOD = 3, GPIOE = 4
#define LED3_PORT                     (HT_GPIOB)
#define LED3_PIN                      (6)


#ifdef __cplusplus
}
#endif

#endif
