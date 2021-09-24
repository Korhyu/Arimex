#include "AL02_switches_leds_mux_driver.h"
//#include "AL00_micro_ht32f52241.h"

// Puertos --------------------------------
#define UI_LED_ENABLE1a_PORT		GPIO_PB
#define UI_LED_ENABLE1b_PORT		GPIO_PB
#define UI_LED_ENABLE2a_PORT		GPIO_PA
#define UI_LED_ENABLE2b_PORT		GPIO_PB
#define UI_SW_ENABLE_PORT 			GPIO_PB

#define UI_LED_ENABLE1a_HT_PORT		HT_GPIOB
#define UI_LED_ENABLE1b_HT_PORT		HT_GPIOB
#define UI_LED_ENABLE2a_HT_PORT		HT_GPIOA
#define UI_LED_ENABLE2b_HT_PORT		HT_GPIOB
#define UI_SW_ENABLE_HT_PORT 		HT_GPIOB

#define UI_IO_1_PORT		GPIO_PC
#define UI_IO_2_PORT		GPIO_PC
#define UI_IO_3_PORT		GPIO_PA
#define UI_IO_4_PORT		GPIO_PB
#define UI_IO_5_PORT		GPIO_PC
#define UI_IO_6_PORT		GPIO_PA
#define UI_IO_1_HT_PORT		HT_GPIOC
#define UI_IO_2_HT_PORT		HT_GPIOC
#define UI_IO_3_HT_PORT		HT_GPIOA
#define UI_IO_4_HT_PORT		HT_GPIOB
#define UI_IO_5_HT_PORT		HT_GPIOC
#define UI_IO_6_HT_PORT		HT_GPIOA

// Pines ----------------------------------
#define UI_LED_ENABLE1a_PIN			GPIO10
#define UI_LED_ENABLE1b_PIN			GPIO14
#define UI_LED_ENABLE2a_PIN	 		GPIO15
#define UI_LED_ENABLE2b_PIN			GPIO0
#define UI_SW_ENABLE_PIN			GPIO13

#define UI_LED_ENABLE1a_HT_PIN		AFIO_PIN_10
#define UI_LED_ENABLE1b_HT_PIN		AFIO_PIN_14
#define UI_LED_ENABLE2a_HT_PIN	 	AFIO_PIN_15
#define UI_LED_ENABLE2b_HT_PIN		AFIO_PIN_0
#define UI_SW_ENABLE_HT_PIN			AFIO_PIN_13

#define UI_IO_1_PIN			GPIO6
#define UI_IO_2_PIN			GPIO7
#define UI_IO_3_PIN			GPIO8
#define UI_IO_4_PIN			GPIO12
#define UI_IO_5_PIN			GPIO3
#define UI_IO_6_PIN			GPIO6
#define UI_IO_1_HT_PIN		GPIO_PIN_6
#define UI_IO_2_HT_PIN		GPIO_PIN_7
#define UI_IO_3_HT_PIN		GPIO_PIN_8
#define UI_IO_4_HT_PIN		GPIO_PIN_12
#define UI_IO_5_HT_PIN		GPIO_PIN_3
#define UI_IO_6_HT_PIN		GPIO_PIN_6


	
//Esta macro configura los pines centrales como GPIO - Se deberia llamar 1 sola vez
#define ui_io_pin_gpio_config()			{AFIO_GPxConfig(UI_IO_1_PORT, UI_IO_1_PIN, AFIO_MODE_DEFAULT);\
										 AFIO_GPxConfig(UI_IO_2_PORT, UI_IO_2_PIN, AFIO_MODE_DEFAULT);\
										 AFIO_GPxConfig(UI_IO_3_PORT, UI_IO_3_PIN, AFIO_MODE_DEFAULT);\
										 AFIO_GPxConfig(UI_IO_4_PORT, UI_IO_4_PIN, AFIO_MODE_1);\
										 AFIO_GPxConfig(UI_IO_5_PORT, UI_IO_5_PIN, AFIO_MODE_DEFAULT);\
										 AFIO_GPxConfig(UI_IO_6_PORT, UI_IO_6_PIN, AFIO_MODE_DEFAULT);}

//Esta macro configura los pines centrales como SALIDAS - Se deberia llamar en cada cambio de estado de la maquina
#define ui_io_output_state()			{GPIO_DirectionConfig(UI_IO_1_HT_PORT, UI_IO_1_HT_PIN, GPIO_DIR_OUT);\
										 GPIO_DirectionConfig(UI_IO_2_HT_PORT, UI_IO_2_HT_PIN, GPIO_DIR_OUT);\
										 GPIO_DirectionConfig(UI_IO_3_HT_PORT, UI_IO_3_HT_PIN, GPIO_DIR_OUT);\
										 GPIO_DirectionConfig(UI_IO_4_HT_PORT, UI_IO_4_HT_PIN, GPIO_DIR_OUT);\
										 GPIO_DirectionConfig(UI_IO_5_HT_PORT, UI_IO_5_HT_PIN, GPIO_DIR_OUT);\
										 GPIO_DirectionConfig(UI_IO_6_HT_PORT, UI_IO_6_HT_PIN, GPIO_DIR_OUT);\
										 GPIO_PullResistorConfig(UI_IO_1_HT_PORT, UI_IO_1_HT_PIN, GPIO_PR_DISABLE);\
										 GPIO_PullResistorConfig(UI_IO_2_HT_PORT, UI_IO_2_HT_PIN, GPIO_PR_DISABLE);\
										 GPIO_PullResistorConfig(UI_IO_3_HT_PORT, UI_IO_3_HT_PIN, GPIO_PR_DISABLE);\
										 GPIO_PullResistorConfig(UI_IO_4_HT_PORT, UI_IO_4_HT_PIN, GPIO_PR_DISABLE);\
										 GPIO_PullResistorConfig(UI_IO_5_HT_PORT, UI_IO_5_HT_PIN, GPIO_PR_DISABLE);\
										 GPIO_PullResistorConfig(UI_IO_6_HT_PORT, UI_IO_6_HT_PIN, GPIO_PR_DISABLE);}
										 /*
										 GPIO_InputConfig(UI_IO_1_HT_PORT, UI_IO_1_HT_PIN, DISABLE);\
										 GPIO_InputConfig(UI_IO_2_HT_PORT, UI_IO_2_HT_PIN, DISABLE);\
										 GPIO_InputConfig(UI_IO_3_HT_PORT, UI_IO_3_HT_PIN, DISABLE);\
										 GPIO_InputConfig(UI_IO_4_HT_PORT, UI_IO_4_HT_PIN, DISABLE);\
										 GPIO_InputConfig(UI_IO_5_HT_PORT, UI_IO_5_HT_PIN, DISABLE);\
										 GPIO_InputConfig(UI_IO_6_HT_PORT, UI_IO_6_HT_PIN, DISABLE);}*/

//Esta macro configura los pines centrales como ENTRADAS - Se deberia llamar en cada cambio de estado de la maquina
#define ui_io_intput_state()			{GPIO_DirectionConfig(UI_IO_1_HT_PORT, UI_IO_1_HT_PIN, GPIO_DIR_IN);\
										 GPIO_DirectionConfig(UI_IO_2_HT_PORT, UI_IO_2_HT_PIN, GPIO_DIR_IN);\
										 GPIO_DirectionConfig(UI_IO_3_HT_PORT, UI_IO_3_HT_PIN, GPIO_DIR_IN);\
										 GPIO_DirectionConfig(UI_IO_4_HT_PORT, UI_IO_4_HT_PIN, GPIO_DIR_IN);\
										 GPIO_DirectionConfig(UI_IO_5_HT_PORT, UI_IO_5_HT_PIN, GPIO_DIR_IN);\
										 GPIO_DirectionConfig(UI_IO_6_HT_PORT, UI_IO_6_HT_PIN, GPIO_DIR_IN);\
										 GPIO_PullResistorConfig(UI_IO_1_HT_PORT, UI_IO_1_HT_PIN, GPIO_PR_DOWN);\
										 GPIO_PullResistorConfig(UI_IO_2_HT_PORT, UI_IO_2_HT_PIN, GPIO_PR_DOWN);\
										 GPIO_PullResistorConfig(UI_IO_3_HT_PORT, UI_IO_3_HT_PIN, GPIO_PR_DOWN);\
										 GPIO_PullResistorConfig(UI_IO_4_HT_PORT, UI_IO_4_HT_PIN, GPIO_PR_DOWN);\
										 GPIO_PullResistorConfig(UI_IO_5_HT_PORT, UI_IO_5_HT_PIN, GPIO_PR_DOWN);\
										 GPIO_PullResistorConfig(UI_IO_6_HT_PORT, UI_IO_6_HT_PIN, GPIO_PR_DOWN);}
										 /*
										 GPIO_InputConfig(UI_IO_1_HT_PORT, UI_IO_1_HT_PIN, ENABLE);\
										 GPIO_InputConfig(UI_IO_2_HT_PORT, UI_IO_2_HT_PIN, ENABLE);\
										 GPIO_InputConfig(UI_IO_3_HT_PORT, UI_IO_3_HT_PIN, ENABLE);\
										 GPIO_InputConfig(UI_IO_4_HT_PORT, UI_IO_4_HT_PIN, ENABLE);\
										 GPIO_InputConfig(UI_IO_5_HT_PORT, UI_IO_5_HT_PIN, ENABLE);\
										 GPIO_InputConfig(UI_IO_6_HT_PORT, UI_IO_6_HT_PIN, ENABLE);}*/

//Configuro los pines bajos de los leds como SALIDAS - Se deberia llamar 1 sola vez
#define ui_leds_output_state()			{AFIO_GPxConfig(UI_LED_ENABLE1a_PORT, UI_LED_ENABLE1a_PIN, AFIO_MODE_1);\
										 AFIO_GPxConfig(UI_LED_ENABLE1b_PORT, UI_LED_ENABLE1b_PIN, AFIO_MODE_1);\
										 AFIO_GPxConfig(UI_LED_ENABLE2a_PORT, UI_LED_ENABLE2a_PIN, AFIO_MODE_DEFAULT);\
										 AFIO_GPxConfig(UI_LED_ENABLE2b_PORT, UI_LED_ENABLE2b_PIN, AFIO_MODE_DEFAULT);\
										 GPIO_DirectionConfig(UI_LED_ENABLE1a_HT_PORT, UI_LED_ENABLE1a_HT_PIN, GPIO_DIR_OUT);\
										 GPIO_DirectionConfig(UI_LED_ENABLE1b_HT_PORT, UI_LED_ENABLE1b_HT_PIN, GPIO_DIR_OUT);\
										 GPIO_DirectionConfig(UI_LED_ENABLE2a_HT_PORT, UI_LED_ENABLE2a_HT_PIN, GPIO_DIR_OUT);\
										 GPIO_DirectionConfig(UI_LED_ENABLE2b_HT_PORT, UI_LED_ENABLE2b_HT_PIN, GPIO_DIR_OUT);\
										 GPIO_PullResistorConfig(UI_LED_ENABLE1a_HT_PORT, UI_LED_ENABLE1a_HT_PIN, GPIO_PR_UP);\
										 GPIO_PullResistorConfig(UI_LED_ENABLE1b_HT_PORT, UI_LED_ENABLE1b_HT_PIN, GPIO_PR_UP);\
										 GPIO_PullResistorConfig(UI_LED_ENABLE2a_HT_PORT, UI_LED_ENABLE2a_HT_PIN, GPIO_PR_UP);\
										 GPIO_PullResistorConfig(UI_LED_ENABLE2b_HT_PORT, UI_LED_ENABLE2b_HT_PIN, GPIO_PR_UP);}		//AFIO_MODE_1

//Configuro el pin alto de los SW como SALIDAS - Se deberia llamar 1 sola vez
#define ui_sw_output_state()			{AFIO_GPxConfig(UI_SW_ENABLE_PORT, UI_SW_ENABLE_PIN, AFIO_MODE_1);\
										 GPIO_DirectionConfig(UI_SW_ENABLE_HT_PORT, UI_SW_ENABLE_HT_PIN, GPIO_DIR_OUT);\
										 GPIO_InputConfig(UI_SW_ENABLE_HT_PORT, UI_SW_ENABLE_HT_PIN, DISABLE);\
										 GPIO_PullResistorConfig(UI_SW_ENABLE_HT_PORT, UI_SW_ENABLE_HT_PIN, GPIO_PR_DISABLE);}

//Configuro en bajo los pines centrales
#define ui_io_reset_state()				{__hardware_gpio_output_reset(UI_IO_1_PORT, UI_IO_1_PIN);\
										 __hardware_gpio_output_reset(UI_IO_2_PORT, UI_IO_2_PIN);\
										 __hardware_gpio_output_reset(UI_IO_3_PORT, UI_IO_3_PIN);\
										 __hardware_gpio_output_reset(UI_IO_4_PORT, UI_IO_4_PIN);\
										 __hardware_gpio_output_reset(UI_IO_5_PORT, UI_IO_5_PIN);\
										 __hardware_gpio_output_reset(UI_IO_6_PORT, UI_IO_6_PIN);}

//Configuro en alto los pines centrales
#define ui_io_set_state()				{__hardware_gpio_output_set(UI_IO_1_PORT, UI_IO_1_PIN);\
										 __hardware_gpio_output_set(UI_IO_2_PORT, UI_IO_2_PIN);\
										 __hardware_gpio_output_set(UI_IO_3_PORT, UI_IO_3_PIN);\
										 __hardware_gpio_output_set(UI_IO_4_PORT, UI_IO_4_PIN);\
										 __hardware_gpio_output_set(UI_IO_5_PORT, UI_IO_5_PIN);\
										 __hardware_gpio_output_set(UI_IO_6_PORT, UI_IO_6_PIN);}

//Configuro en alto los pines bajos de los LEDS
#define ui_leds_set_state()				{__hardware_gpio_output_set(BOARD_UI_LED_ENABLE1a_PORT, BOARD_UI_LED_ENABLE1a_PIN);\
										 __hardware_gpio_output_set(BOARD_UI_LED_ENABLE1b_PORT, BOARD_UI_LED_ENABLE1b_PIN);\
										 __hardware_gpio_output_set(BOARD_UI_LED_ENABLE2a_PORT, BOARD_UI_LED_ENABLE2a_PIN);\
										 __hardware_gpio_output_set(BOARD_UI_LED_ENABLE2b_PORT, BOARD_UI_LED_ENABLE2b_PIN);}

//Configuro en bajo los pines bajos de los LEDS
#define ui_leds_reset_state()			{__hardware_gpio_output_reset(BOARD_UI_LED_ENABLE1a_PORT, BOARD_UI_LED_ENABLE1a_PIN);\
										 __hardware_gpio_output_reset(BOARD_UI_LED_ENABLE1b_PORT, BOARD_UI_LED_ENABLE1b_PIN);\
										 __hardware_gpio_output_reset(BOARD_UI_LED_ENABLE2a_PORT, BOARD_UI_LED_ENABLE2a_PIN);\
										 __hardware_gpio_output_reset(BOARD_UI_LED_ENABLE2b_PORT, BOARD_UI_LED_ENABLE2b_PIN);}

//Configuro en bajo el pin alto de los SW
#define ui_sw_reset_state()				{__hardware_gpio_output_reset(BOARD_UI_SWITCH_ENABLE_PORT, BOARD_UI_SWITCH_ENABLE_PIN);}

//Configuro en alto el pin alto de los SW
#define ui_sw_set_state()				{__hardware_gpio_output_set(BOARD_UI_SWITCH_ENABLE_PORT, BOARD_UI_SWITCH_ENABLE_PIN);}


/*
// CODIGO VIEJO ---------------------------------------------------------------
#define ui_mux_outputs_state_init()		{__hardware_gpio_output_reset(BOARD_UI_LED_ENABLE1a_PORT,BOARD_UI_LED_ENABLE1a_PIN);\
										 __hardware_gpio_output_reset(BOARD_UI_LED_ENABLE1b_PORT,BOARD_UI_LED_ENABLE1b_PIN);\
										 __hardware_gpio_output_reset(BOARD_UI_LED_ENABLE2a_PORT,BOARD_UI_LED_ENABLE2a_PIN);\
										 __hardware_gpio_output_reset(BOARD_UI_LED_ENABLE2b_PORT,BOARD_UI_LED_ENABLE2b_PIN);\
										 __hardware_gpio_output_set(BOARD_UI_SWITCH_ENABLE_PORT,BOARD_UI_SWITCH_ENABLE_PIN);}

#define ui_mux_leds_comm_enable()		{AFIO_GPxConfig(GPIO_PA, BOARD_UI_LED_ENABLE2a_PIN, 0);\
										 AFIO_GPxConfig(GPIO_PB, BOARD_UI_LED_ENABLE1a_PIN, 1);\
										 AFIO_GPxConfig(GPIO_PB, BOARD_UI_LED_ENABLE1b_PIN, 1);\
										 AFIO_GPxConfig(GPIO_PB, BOARD_UI_LED_ENABLE2b_PIN, 0);\
										 __hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_LED_ENABLE1a_PORT,BOARD_UI_LED_ENABLE1a_PIN);\
										 __hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_LED_ENABLE1b_PORT,BOARD_UI_LED_ENABLE1b_PIN);\
										 __hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_LED_ENABLE2a_PORT,BOARD_UI_LED_ENABLE2a_PIN);\
										 __hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_LED_ENABLE2b_PORT,BOARD_UI_LED_ENABLE2b_PIN);}

#define ui_mux_leds_comm_disable() 	   {__hardware_gpio_config_set_as_hiz_pin(BOARD_UI_LED_ENABLE1a_PORT,BOARD_UI_LED_ENABLE1a_PIN);\
										 __hardware_gpio_config_set_as_hiz_pin(BOARD_UI_LED_ENABLE1b_PORT,BOARD_UI_LED_ENABLE1b_PIN);\
										 __hardware_gpio_config_set_as_hiz_pin(BOARD_UI_LED_ENABLE2a_PORT,BOARD_UI_LED_ENABLE2a_PIN);\
										 __hardware_gpio_config_set_as_hiz_pin(BOARD_UI_LED_ENABLE2b_PORT,BOARD_UI_LED_ENABLE2b_PIN);}
	
#define ui_mux_leds_comm_set_high()		{__hardware_gpio_output_set(BOARD_UI_LED_ENABLE1a_PORT,BOARD_UI_LED_ENABLE1a_PIN);\
										 __hardware_gpio_output_set(BOARD_UI_LED_ENABLE1b_PORT,BOARD_UI_LED_ENABLE1b_PIN);\
										 __hardware_gpio_output_set(BOARD_UI_LED_ENABLE2a_PORT,BOARD_UI_LED_ENABLE2a_PIN);\
										 __hardware_gpio_output_set(BOARD_UI_LED_ENABLE2b_PORT,BOARD_UI_LED_ENABLE2b_PIN);}

#define ui_mux_leds_comm_set_low()		{__hardware_gpio_output_reset(BOARD_UI_LED_ENABLE1a_PORT,BOARD_UI_LED_ENABLE1a_PIN);\
										 __hardware_gpio_output_reset(BOARD_UI_LED_ENABLE1b_PORT,BOARD_UI_LED_ENABLE1b_PIN);\
										 __hardware_gpio_output_reset(BOARD_UI_LED_ENABLE2a_PORT,BOARD_UI_LED_ENABLE2a_PIN);\
										 __hardware_gpio_output_reset(BOARD_UI_LED_ENABLE2b_PORT,BOARD_UI_LED_ENABLE2b_PIN);}

#define ui_mux_switches_comm_set_low()	{__hardware_gpio_output_reset(BOARD_UI_SWITCH_ENABLE_PORT,BOARD_UI_SWITCH_ENABLE_PIN);}

#define ui_mux_switches_comm_set_high()	{__hardware_gpio_output_set(BOARD_UI_SWITCH_ENABLE_PORT,BOARD_UI_SWITCH_ENABLE_PIN);}
																				
#define ui_mux_switches_comm_enable()	{__hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_SWITCH_ENABLE_PORT,BOARD_UI_SWITCH_ENABLE_PIN);\
										 __hardware_gpio_output_set(BOARD_UI_SWITCH_ENABLE_PORT,BOARD_UI_SWITCH_ENABLE_PIN);} 

#define ui_mux_switches_comm_disable() 	{__hardware_gpio_config_set_as_hiz_pin(BOARD_UI_SWITCH_ENABLE_PORT,BOARD_UI_SWITCH_ENABLE_PIN);\
										 __hardware_gpio_output_reset(BOARD_UI_SWITCH_ENABLE_PORT,BOARD_UI_SWITCH_ENABLE_PIN);}

#define ui_mux_io_bus_set_as_output()	{__hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_IO_1_PORT,BOARD_UI_IO_1_PIN);\
										 __hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_IO_2_PORT,BOARD_UI_IO_2_PIN);\
										 __hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_IO_3_PORT,BOARD_UI_IO_3_PIN);\
										 __hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_IO_4_PORT,BOARD_UI_IO_4_PIN);\
										 __hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_IO_5_PORT,BOARD_UI_IO_5_PIN);\
										 __hardware_gpio_config_set_as_pushpull_output_pin(BOARD_UI_IO_6_PORT,BOARD_UI_IO_6_PIN);}

#define ui_mux_io_bus_set_as_input()	{__hardware_gpio_config_set_as_hiz_pin(BOARD_UI_IO_1_PORT,BOARD_UI_IO_1_PIN);\
										 __hardware_gpio_config_set_as_hiz_pin(BOARD_UI_IO_2_PORT,BOARD_UI_IO_2_PIN);\
										 __hardware_gpio_config_set_as_hiz_pin(BOARD_UI_IO_3_PORT,BOARD_UI_IO_3_PIN);\
										 __hardware_gpio_config_set_as_hiz_pin(BOARD_UI_IO_4_PORT,BOARD_UI_IO_4_PIN);\
										 __hardware_gpio_config_set_as_hiz_pin(BOARD_UI_IO_5_PORT,BOARD_UI_IO_5_PIN);\
										 __hardware_gpio_config_set_as_hiz_pin(BOARD_UI_IO_6_PORT,BOARD_UI_IO_6_PIN);}

//#define ui_mux_io_bus_set_as_output()	{AFIO_GPxConfig(GPIO_PC, BOARD_UI_IO_1_PIN, 0);\
										 AFIO_GPxConfig(GPIO_PC, BOARD_UI_IO_2_PIN, 0);\
										 AFIO_GPxConfig(GPIO_PA, BOARD_UI_IO_3_PIN, 0);\
										 AFIO_GPxConfig(GPIO_PB, BOARD_UI_IO_4_PIN, 1);\
										 GPIO_DirectionConfig(BOARD_UI_IO_1_PORT, BOARD_UI_IO_1_PIN, GPIO_DIR_OUT);\
										 GPIO_DirectionConfig(BOARD_UI_IO_2_PORT, BOARD_UI_IO_2_PIN, GPIO_DIR_OUT);\
										 GPIO_DirectionConfig(BOARD_UI_IO_3_PORT, BOARD_UI_IO_3_PIN, GPIO_DIR_OUT);\
										 GPIO_DirectionConfig(BOARD_UI_IO_4_PORT, BOARD_UI_IO_4_PIN, GPIO_DIR_OUT);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_1_PORT, BOARD_UI_IO_1_PIN, GPIO_PR_DISABLE);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_2_PORT, BOARD_UI_IO_2_PIN, GPIO_PR_DISABLE);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_3_PORT, BOARD_UI_IO_3_PIN, GPIO_PR_DISABLE);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_4_PORT, BOARD_UI_IO_3_PIN, GPIO_PR_DISABLE);\
										 GPIO_InputConfig(BOARD_UI_IO_1_PORT, BOARD_UI_IO_1_PIN, DISABLE);\
										 GPIO_InputConfig(BOARD_UI_IO_2_PORT, BOARD_UI_IO_2_PIN, DISABLE);\
										 GPIO_InputConfig(BOARD_UI_IO_3_PORT, BOARD_UI_IO_3_PIN, DISABLE);\
										 GPIO_InputConfig(BOARD_UI_IO_4_PORT, BOARD_UI_IO_4_PIN, DISABLE);}

										

//#define ui_mux_io_bus_set_as_input()	{AFIO_GPxConfig(GPIO_PC, BOARD_UI_IO_1_PIN, 0);\
										 AFIO_GPxConfig(GPIO_PC, BOARD_UI_IO_2_PIN, 0);\
										 AFIO_GPxConfig(GPIO_PA, BOARD_UI_IO_3_PIN, 0);\
										 AFIO_GPxConfig(GPIO_PB, BOARD_UI_IO_4_PIN, 1);\
										 GPIO_DirectionConfig(BOARD_UI_IO_1_PORT, BOARD_UI_IO_1_PIN, GPIO_DIR_IN);\
										 GPIO_DirectionConfig(BOARD_UI_IO_2_PORT, BOARD_UI_IO_2_PIN, GPIO_DIR_IN);\
										 GPIO_DirectionConfig(BOARD_UI_IO_3_PORT, BOARD_UI_IO_3_PIN, GPIO_DIR_IN);\
										 GPIO_DirectionConfig(BOARD_UI_IO_4_PORT, BOARD_UI_IO_4_PIN, GPIO_DIR_IN);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_1_PORT, BOARD_UI_IO_1_PIN, GPIO_PR_DOWN);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_2_PORT, BOARD_UI_IO_2_PIN, GPIO_PR_DOWN);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_3_PORT, BOARD_UI_IO_3_PIN, GPIO_PR_DOWN);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_4_PORT, BOARD_UI_IO_3_PIN, GPIO_PR_DOWN);\
										 GPIO_InputConfig(BOARD_UI_IO_1_PORT, BOARD_UI_IO_1_PIN, ENABLE);\
										 GPIO_InputConfig(BOARD_UI_IO_2_PORT, BOARD_UI_IO_2_PIN, ENABLE);\
										 GPIO_InputConfig(BOARD_UI_IO_3_PORT, BOARD_UI_IO_3_PIN, ENABLE);\
										 GPIO_InputConfig(BOARD_UI_IO_4_PORT, BOARD_UI_IO_4_PIN, ENABLE);}

#define config_gpio_switch_pulldown()	{GPIO_PullResistorConfig(BOARD_UI_IO_1_PORT, BOARD_UI_IO_1_PIN, GPIO_PR_DOWN);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_2_PORT, BOARD_UI_IO_2_PIN, GPIO_PR_DOWN);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_3_PORT, BOARD_UI_IO_3_PIN, GPIO_PR_DOWN);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_4_PORT, BOARD_UI_IO_4_PIN, GPIO_PR_DOWN);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_5_PORT, BOARD_UI_IO_5_PIN, GPIO_PR_DOWN);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_6_PORT, BOARD_UI_IO_6_PIN, GPIO_PR_DOWN);}

#define config_gpio_switch_pullup()		{GPIO_PullResistorConfig(BOARD_UI_IO_1_PORT, BOARD_UI_IO_1_PIN, GPIO_PR_UP);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_2_PORT, BOARD_UI_IO_2_PIN, GPIO_PR_UP);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_3_PORT, BOARD_UI_IO_3_PIN, GPIO_PR_UP);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_4_PORT, BOARD_UI_IO_4_PIN, GPIO_PR_UP);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_5_PORT, BOARD_UI_IO_5_PIN, GPIO_PR_UP);\
										 GPIO_PullResistorConfig(BOARD_UI_IO_6_PORT, BOARD_UI_IO_6_PIN, GPIO_PR_UP);}

#define config_gpio_switch_pull_disable()	{GPIO_PullResistorConfig(BOARD_UI_IO_1_PORT, BOARD_UI_IO_1_PIN, GPIO_PR_DISABLE);\
											 GPIO_PullResistorConfig(BOARD_UI_IO_2_PORT, BOARD_UI_IO_2_PIN, GPIO_PR_DISABLE);\
											 GPIO_PullResistorConfig(BOARD_UI_IO_3_PORT, BOARD_UI_IO_3_PIN, GPIO_PR_DISABLE);\
											 GPIO_PullResistorConfig(BOARD_UI_IO_4_PORT, BOARD_UI_IO_4_PIN, GPIO_PR_DISABLE);\
											 GPIO_PullResistorConfig(BOARD_UI_IO_5_PORT, BOARD_UI_IO_5_PIN, GPIO_PR_DISABLE);\
											 GPIO_PullResistorConfig(BOARD_UI_IO_6_PORT, BOARD_UI_IO_6_PIN, GPIO_PR_DISABLE);}


// Fin Codigo Viejo ---------------------------------------------------------------------------------------------------
*/

#define ui_led_max_fan_set_on()			__hardware_gpio_output_set	(BOARD_UI_IO_6_PORT,BOARD_UI_IO_6_PIN)
#define ui_led_max_fan_set_off()		__hardware_gpio_output_reset(BOARD_UI_IO_6_PORT,BOARD_UI_IO_6_PIN)
#define ui_led_mid_fan_set_on()			__hardware_gpio_output_set	(BOARD_UI_IO_5_PORT,BOARD_UI_IO_5_PIN)
#define ui_led_mid_fan_set_off()		__hardware_gpio_output_reset(BOARD_UI_IO_5_PORT,BOARD_UI_IO_5_PIN)
#define ui_led_min_fan_set_on()			__hardware_gpio_output_set	(BOARD_UI_IO_4_PORT,BOARD_UI_IO_4_PIN)
#define ui_led_min_fan_set_off()		__hardware_gpio_output_reset(BOARD_UI_IO_4_PORT,BOARD_UI_IO_4_PIN)
		
#define ui_led_max_heat_set_on()		__hardware_gpio_output_set	(BOARD_UI_IO_3_PORT,BOARD_UI_IO_3_PIN)
#define ui_led_max_heat_set_off()		__hardware_gpio_output_reset(BOARD_UI_IO_3_PORT,BOARD_UI_IO_3_PIN)
#define ui_led_mid_heat_set_on()		__hardware_gpio_output_set	(BOARD_UI_IO_2_PORT,BOARD_UI_IO_2_PIN)
#define ui_led_mid_heat_set_off()		__hardware_gpio_output_reset(BOARD_UI_IO_2_PORT,BOARD_UI_IO_2_PIN)
#define ui_led_min_heat_set_on()		__hardware_gpio_output_set	(BOARD_UI_IO_1_PORT,BOARD_UI_IO_1_PIN)
#define ui_led_min_heat_set_off()		__hardware_gpio_output_reset(BOARD_UI_IO_1_PORT,BOARD_UI_IO_1_PIN)

#define ui_led_min_heat_toggle()		__hardware_gpio_output_toggle(BOARD_UI_IO_1_PORT,BOARD_UI_IO_1_PIN)
#define ui_led_mid_heat_toggle()		__hardware_gpio_output_toggle(BOARD_UI_IO_2_PORT,BOARD_UI_IO_2_PIN)
#define ui_led_max_heat_toggle()		__hardware_gpio_output_toggle(BOARD_UI_IO_3_PORT,BOARD_UI_IO_3_PIN)
#define ui_led_min_fan_toggle()			__hardware_gpio_output_toggle(BOARD_UI_IO_4_PORT,BOARD_UI_IO_4_PIN)
#define ui_led_mid_fan_toggle()			__hardware_gpio_output_toggle(BOARD_UI_IO_5_PORT,BOARD_UI_IO_5_PIN)
#define ui_led_max_fan_toggle()			__hardware_gpio_output_toggle(BOARD_UI_IO_6_PORT,BOARD_UI_IO_6_PIN)

#define ui_switch_coldshot_get_state()	__hardware_gpio_input_read_state(BOARD_UI_IO_6_PORT,BOARD_UI_IO_6_PIN)
#define ui_switch_lock_get_state()		__hardware_gpio_input_read_state(BOARD_UI_IO_5_PORT,BOARD_UI_IO_5_PIN)
#define ui_switch_inc_fan_get_state()	__hardware_gpio_input_read_state(BOARD_UI_IO_4_PORT,BOARD_UI_IO_4_PIN)
#define ui_switch_dec_fan_get_state()	__hardware_gpio_input_read_state(BOARD_UI_IO_3_PORT,BOARD_UI_IO_3_PIN)																				
#define ui_switch_inc_heat_get_state()	__hardware_gpio_input_read_state(BOARD_UI_IO_2_PORT,BOARD_UI_IO_2_PIN)						
#define ui_switch_dec_heat_get_state()	__hardware_gpio_input_read_state(BOARD_UI_IO_1_PORT,BOARD_UI_IO_1_PIN)		



volatile static uint8_t switches_state=0, leds_state=0x00;
uint8_t ui_timer_flag=UI_TIMER_EXPIRED;


/*
 * Esta funcion devuelve el estado de la variable global actual_state
 */
uint8_t ui_get_flag_state (void)
{
	return ui_timer_flag;
}

/*
 * Esta funcion escribe el estado de la variable global actual_state
 */
void ui_set_flag_state (uint8_t new_state)
{
	ui_timer_flag = new_state;
}

/*
 * Esta funcion retorna el ultimo sensado de los switches
 */
uint8_t ui_mux_get_switches_state (void)
{
	return switches_state;
}

/*
 * Esta funcion retorna el estado de los leds: usar los shift_mask para encuestar cada led
 */
uint8_t ui_mux_get_leds_state(void)
{
	return leds_state;
}

/*
 * Inicializacion de tareas y linkeo de callbacks
 */
void ui_mux_init (void)
{
	//ui_mux_outputs_state_init();
	//board_timer_link_callback(ui_mux_driver_routine,BOARD_TIM_UI_SEL_CALLBACK);
	//ui_mux_driver_routine();
	board_timer_link_callback(ui_mux_callback,BOARD_TIM_UI_SEL_CALLBACK);

	ui_io_pin_gpio_config();		//Configuro los pines medios como GPIO
	ui_leds_output_state();			//Configurlo los pines bajos de los leds como GPIO y salidas
	ui_sw_output_state();			//Configuro el pin alto de los SW como GPIO y salida

	//ui_timer_set_irq_within_us(MUX_TIME_SWITCHES_us);	//Cargo el tiempo que quiero leer los SW
}


/*
 * Setea el valor de la variable leds. Para observar el cambio es necesario estar llamando a "ui_mux_leds_state_update"
 */
void ui_mux_set_leds_state	(uint8_t new_state)
{
	leds_state = new_state;
	ui_mux_leds_state_update();
}

/*
 * Actualiza los registros del puerto que encienden los leds (debe llamarse periodicamente como task)
 */
void	ui_mux_leds_state_update (void)
{
	if(leds_state&(1<<UI_LED_MIN_HEAT_SHIFT_MASK))
		ui_led_min_heat_set_on();
	else
		ui_led_min_heat_set_off();
	
	if(leds_state&(1<<UI_LED_MID_HEAT_SHIFT_MASK))
		ui_led_mid_heat_set_on();
	else
		ui_led_mid_heat_set_off();
	
	if(leds_state&(1<<UI_LED_MAX_HEAT_SHIFT_MASK))
		ui_led_max_heat_set_on();
	else
		ui_led_max_heat_set_off();

	if(leds_state&(1<<UI_LED_MIN_FAN_SHIFT_MASK))
		ui_led_min_fan_set_on();
	else
		ui_led_min_fan_set_off();
	
	if(leds_state&(1<<UI_LED_MID_FAN_SHIFT_MASK))
		ui_led_mid_fan_set_on();
	else
		ui_led_mid_fan_set_off();
	
	if(leds_state&(1<<UI_LED_MAX_FAN_SHIFT_MASK))
		ui_led_max_fan_set_on();
	else
		ui_led_max_fan_set_off();
}


/*
 * Esta funcion se encarga de multiplexar leds y switches compartidos en el mismo GPIO. 
 * Debe llamarse periodicamente como un callback de una irq de timer. 
 */
/*
void ui_mux_driver_routine (void)
{
	switch(actual_state)
	{
		case UI_STATE_RESET:
			ui_io_pin_gpio_config();		//Configuro los pines medios como GPIO
			ui_leds_output_state();			//Configurlo los pines bajos de los leds como GPIO y salidas
			ui_sw_output_state();			//Configuro el pin alto de los SW como GPIO y salida
			
		
		case UI_WAITING_TIMER_SWITCHES:
			//Indico que los switches estan listos para ser leidos
			actual_state = UI_STATE_SWITCHES;
				
			break;	
		
		case UI_WAITING_TIMER_LEDS:
			//Indico que los LEDs estan listos para ser escritos
			actual_state = UI_STATE_LEDS;

			break;
		
		default:
			break;		
	}
}
*/

/* ----------- Codigo Jose ----------- */
uint8_t switches_status (void)
{
	//Funcion que analiza el resultado de los switches e informa si hubo cambios respecto al estado anterior
	//Luego de realizar la funcion correspondiete deberia cambiar el estado global de la funcion switches para evitar entrar de nuevo

	static uint8_t switches_states = 0;						//Es el estado actual de los switches, la ultima funcion que se ejecuto

	if(switches_states != ui_mux_get_switches_state())
	{
		//Actualizo el estado de los switches
		switches_states = ui_mux_get_switches_state();
		
		//Hubo un cambio en los switches busco en que switch se efectuo el cambio
		if ( (switches_states & UI_SWITCH_COLDSHOT_SHIFT_MASK) == TRUE )
			//Enciendo el coldshot
			return COLDSHOT_ON_SW;
		else
			//Apago el cold shot
			return COLDSHOT_OFF_SW;
		
		if ( (switches_states & UI_SWITCH_LOCK_SHIFT_MASK) == TRUE )
			//Lock activado
			return LOCK_ON_SW;
		else
			//Lock desactivado
			return LOCK_OFF_SW;

		if ( (switches_states & UI_SWITCH_INC_FAN_SHIFT_MASK) == TRUE )
			//Aumentar velocidad de motor
			return MOTOR_UP_SW;			
		
		if ( (switches_states & UI_SWITCH_DEC_FAN_SHIFT_MASK) == TRUE )
			//Disminuir velocidad de motor
			return MOTOR_DOWN_SW;

		if ( (switches_states & UI_SWITCH_INC_HEAT_SHIFT_MASK) == TRUE )
			//Aumentar velocidad de motor
			return HEAT_UP_SW;
		
		if ( (switches_states & UI_SWITCH_DEC_HEAT_SHIFT_MASK) == TRUE )
			//Disminuir velocidad de motor
			return HEAT_DOWN_SW;
	}
	else
	{
		return NO_CHANGE_SW;			//No hay cambio respecto al estado anterior de los switches
	}
}

void ui_mux_callback (void)
{
	ui_timer_flag = UI_TIMER_EXPIRED;
}

void change_state_to_switches (void)
{
	ui_sw_set_state();					//Pongo el pin alto del SW en set para leer el estado alto cuando se pulsa un pulsador
	ui_leds_set_state();				//Pongo los pines bajos de los LEDS en set para evitar que se enciendan cuando se pulsa un pulsador
	ui_io_intput_state();				//Configuro los pines medios como entradas.
}

void change_state_to_leds (void)
{
	//Dejo todo listo para los LEDS
	ui_sw_reset_state();				//Pongo el pin alto del SW en reset para evitar que si se presiona se enciendan los LEDS
	ui_leds_reset_state();				//Pongo los pines bajos de los LEDS en reset para poder encenderlos
	ui_io_output_state();				//Configuro los pines medios como SALIDAS
}

void read_switches_state (void)
{
	switches_state =	(ui_switch_coldshot_get_state()	<<	UI_SWITCH_COLDSHOT_SHIFT_MASK)  |
						(ui_switch_lock_get_state()		<<	UI_SWITCH_LOCK_SHIFT_MASK)		|
						(ui_switch_inc_fan_get_state()	<<	UI_SWITCH_INC_FAN_SHIFT_MASK)	|
						(ui_switch_dec_fan_get_state()	<<	UI_SWITCH_DEC_FAN_SHIFT_MASK)	|
						(ui_switch_inc_heat_get_state()	<<	UI_SWITCH_INC_HEAT_SHIFT_MASK)  |
						(ui_switch_dec_heat_get_state()	<<	UI_SWITCH_DEC_HEAT_SHIFT_MASK);
}