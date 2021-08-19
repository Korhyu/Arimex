/* Includes ------------------------------------------------------------------------------------------------*/
#include "AL01_board_iq_aed_rev20191011.h"
#include "AL01_flash_management.h"
#include "AL02_inverter_3phase.h"
#include "AL02_switches_leds_mux_driver.h"
#include "AL03_motor_3phase_driver.h"
#include "Jose.h"

/* Global functions ----------------------------------------------------------------------------------------*/
/* Settings ------------------------------------------------------------------------------------------------*/
/* Private types -------------------------------------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------------------------------------*/
/* Global variables ----------------------------------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------------------------------------*/
/* Global functions ----------------------------------------------------------------------------------------*/


/*********************************************************************************************************//**
  * @brief  Main program.
  * @retval None
  ***********************************************************************************************************/
#define TIM 				2000

#define COM1_TX_GPIO_ID             (GPIO_PA)
#define COM1_TX_AFIO_PIN            (AFIO_PIN_4)
#define COM1_TX_AFIO_MODE           (AFIO_FUN_USART_UART)

#define COM1_RX_GPIO_ID             (GPIO_PA)
#define COM1_RX_AFIO_PIN            (AFIO_PIN_5)
#define COM1_RX_AFIO_MODE           (AFIO_FUN_USART_UART)


void board_UART_config(void)
{
	USART_InitTypeDef USART_InitStructure;


	/* Enable peripheral clock of AFIO, USART0                                                                */
	CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
	CKCUClock.Bit.AFIO   = 1;
	COM1_CLK(CKCUClock)  = 1;
	CKCU_PeripClockConfig(CKCUClock, ENABLE);

	/* Config AFIO mode as USART0_Rx and USART0_Tx function.                                                  */
	AFIO_GPxConfig(COM1_TX_GPIO_ID, COM1_TX_AFIO_PIN, AFIO_FUN_USART_UART);
	AFIO_GPxConfig(COM1_RX_GPIO_ID, COM1_RX_AFIO_PIN, AFIO_FUN_USART_UART);

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WORDLENGTH_8B;
	USART_InitStructure.USART_StopBits = USART_STOPBITS_1;
	USART_InitStructure.USART_Parity = USART_PARITY_NO;
	USART_InitStructure.USART_Mode = USART_MODE_NORMAL;

	USART_Init(COM1_PORT, &USART_InitStructure);
	USART_TxCmd(COM1_PORT, ENABLE);
	USART_RxCmd(COM1_PORT, ENABLE);
}


int main(void)
{	
	board_hardware_configuration();
	//board_UART_config();


	uint32_t timer=0;
	

	timer = board_scheduler_load_timer(TIM);
	
	
	printf("Borro bien la flash\n");
	printf("Hubo un error al borrar la flash\n");
	printf("flash vacia\n");
	printf("flash no vacia\n");
	
	printf("flash vacia\n");
	printf("flash no vacia\n");
	
	

	while(1)
	{
		if(board_scheduler_is_time_expired(timer))
		{			
			timer = board_scheduler_load_timer(TIM);
		
			printf("Estoy vivo y el systick esta en:%d\n\r", timer);
		}
	}
}