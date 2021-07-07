/* Includes ------------------------------------------------------------------------------------------------*/
#include "AL00_micro_ht32f52241.h"
#include "AL01_board_iq_aed_rev20191011.h"
#include "AL01_flash_management.h"
#include "AL02_switches_leds_mux_driver.h"
#include "AL03_motor_3phase_driver.h"
#include "Jose.h"


void MCTM_Configuration(void)
{
  TM_TimeBaseInitTypeDef MCTM_TimeBaseInitStructure;
  TM_OutputInitTypeDef MCTM_OutputInitStructure;
  MCTM_CHBRKCTRInitTypeDef MCTM_CHBRKCTRInitStructure;

  CKCU_PeripClockConfig_TypeDef CKCUClock = {{ 0 }};
  CKCUClock.Bit.AFIO       = 1;
  CKCUClock.Bit.MCTM0      = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);

  /* Configure MCTM Channel 0 output pins                                                                    */
  AFIO_GPxConfig(GPIO_PA, AFIO_PIN_14, AFIO_FUN_MCTM_GPTM);		//PIN de medicion
	AFIO_GPxConfig(GPIO_PC, AFIO_PIN_2, AFIO_FUN_MCTM_GPTM);		//LED en ESK32
	
	
	/* Configure MCTM Break pin                                                                               */
  AFIO_GPxConfig(GPIO_PB, AFIO_PIN_4, AFIO_FUN_MCTM_GPTM);

  /* MCTM Time Base configuration                                                                           */
  MCTM_TimeBaseInitStructure.CounterReload = HTCFG_MCTM_RELOAD-1;
  MCTM_TimeBaseInitStructure.Prescaler = 0;
  MCTM_TimeBaseInitStructure.RepetitionCounter = 0;
  MCTM_TimeBaseInitStructure.CounterMode = TM_CNT_MODE_UP;
  MCTM_TimeBaseInitStructure.PSCReloadTime = TM_PSC_RLD_IMMEDIATE;
  TM_TimeBaseInit(HT_MCTM0, &MCTM_TimeBaseInitStructure);

  /* MCTM Channel 0, 1, 2 output configuration                                                              */
  MCTM_OutputInitStructure.Channel = TM_CH_0;
  MCTM_OutputInitStructure.OutputMode = TM_OM_PWM1;
  MCTM_OutputInitStructure.Control = TM_CHCTL_ENABLE;
  MCTM_OutputInitStructure.ControlN = TM_CHCTL_ENABLE;
  MCTM_OutputInitStructure.Polarity = TM_CHP_NONINVERTED;
  MCTM_OutputInitStructure.PolarityN = TM_CHP_NONINVERTED;
  MCTM_OutputInitStructure.IdleState = MCTM_OIS_LOW;
  MCTM_OutputInitStructure.IdleStateN = MCTM_OIS_HIGH;
  MCTM_OutputInitStructure.Compare = HTCFG_MCTM_RELOAD * DUTY;
  MCTM_OutputInitStructure.AsymmetricCompare = 0;
  TM_OutputInit(HT_MCTM0, &MCTM_OutputInitStructure);
	
  /* MCTM Off State, lock, Break, Automatic Output enable, dead time configuration                          */
	MCTM_CHBRKCTRInitStructure.OSSRState = MCTM_OSSR_STATE_ENABLE;
  MCTM_CHBRKCTRInitStructure.OSSIState = MCTM_OSSI_STATE_ENABLE;
  MCTM_CHBRKCTRInitStructure.LockLevel = MCTM_LOCK_LEVEL_2;
  MCTM_CHBRKCTRInitStructure.Break0 = MCTM_BREAK_ENABLE;
  MCTM_CHBRKCTRInitStructure.Break0Polarity = MCTM_BREAK_POLARITY_LOW;
  MCTM_CHBRKCTRInitStructure.AutomaticOutput = MCTM_CHAOE_ENABLE;
  MCTM_CHBRKCTRInitStructure.DeadTime = HTCFG_MCTM_DEAD_TIME;
  MCTM_CHBRKCTRInitStructure.BreakFilter = 0;
  MCTM_CHBRKCTRConfig(HT_MCTM0, &MCTM_CHBRKCTRInitStructure);
	
  /* MCTM counter enable                                                                                    */
  TM_Cmd(HT_MCTM0, ENABLE);

  /* MCTM Channel Main Output enable                                                                        */
  MCTM_CHMOECmd(HT_MCTM0, ENABLE);
}



void GPIO_Config (void)
{
	//board_hardware_gpio_config_output_pp_pins_load_config		(GPIOC,GPIO_PIN_2|GPIO_PIN_3);
	//board_hardware_gpio_config_output_pp_pins_load_config		(GPIOC,GPIO_PIN_3);		//LED PCB
	
	//GPIO
	board_hardware_gpio_config_output_pp_pins_load_config		(GPIOA,GPIO_PIN_0|GPIO_PIN_3);		//lin1
	board_hardware_gpio_config_output_pp_pins_load_config		(GPIOB,GPIO_PIN_6|GPIO_PIN_11);		//lin2
	board_hardware_gpio_config_output_pp_pins_load_config		(GPIOC,GPIO_PIN_2);								//lin3
	

	
	
	//EXTI PINS
	//board_hardware_gpio_config_edge_events_pins_load_config	(GPIOB,GPIOB_PIN_1_EDGE|GPIOB_PIN_3_EDGE|GPIOB_PIN_5_EDGE);
	//board_hardware_gpio_config_edge_events_pins_load_config	(GPIOB,GPIOB_PIN_1_EDGE);
	//board_hardware_gpio_config_edge_events_pins_load_config	(GPIOB,GPIOB_PIN_3_EDGE);
	//board_hardware_gpio_config_edge_events_pins_load_config	(GPIOB,GPIOB_PIN_5_EDGE);
	
	//board_hardware_configuration();
}


uint16_t CambioEstado (uint16_t estadoActual)
{
	uint16_t inverter_actual_comm_seq;
	switch (estadoActual)
	{
		case INVERTER_COMM_SEQ1:
								inverter_actual_comm_seq = INVERTER_COMM_SEQ1;

								board_pwm_hin3_disable();		//OUT3: HIZ --- CH1 OSC
								board_pwm_lin3_disable();

								board_pwm_lin1_disable();		//OUT1: PWM(VBUS)
								board_pwm_hin1_enable();

								board_pwm_hin2_disable();		//OUT2: GND --- CH3 OSC
								board_pwm_lin2_enable();
		
								return INVERTER_COMM_SEQ2;
								
								break;

		case INVERTER_COMM_SEQ2:
								inverter_actual_comm_seq = INVERTER_COMM_SEQ2;

								board_pwm_hin2_disable();		//OUT2: HIZ
								board_pwm_lin2_disable();

								board_pwm_lin1_disable();		//OUT1: PWM(VBUS)
								board_pwm_hin1_enable();

								board_pwm_hin3_disable();		//OUT3: GND
								board_pwm_lin3_enable();

								return INVERTER_COMM_SEQ3;
		
								break;

		case INVERTER_COMM_SEQ3:
								inverter_actual_comm_seq = INVERTER_COMM_SEQ3;

								board_pwm_hin1_disable();
								board_pwm_lin1_disable();

								board_pwm_lin2_disable();
								board_pwm_hin2_enable();

								board_pwm_hin3_disable();
								board_pwm_lin3_enable();
								
								return INVERTER_COMM_SEQ4;
		
								break;

		case INVERTER_COMM_SEQ4:
								inverter_actual_comm_seq = INVERTER_COMM_SEQ4;

								board_pwm_hin3_disable();
								board_pwm_lin3_disable();

								board_pwm_lin2_disable();
								board_pwm_hin2_enable();

								board_pwm_hin1_disable();
								board_pwm_lin1_enable();
	
								return INVERTER_COMM_SEQ5;
		
								break;

		case INVERTER_COMM_SEQ5:
								inverter_actual_comm_seq = INVERTER_COMM_SEQ5;

								board_pwm_hin2_disable();
								board_pwm_lin2_disable();

								board_pwm_lin3_disable();
								board_pwm_hin3_enable();

								board_pwm_hin1_disable();
								board_pwm_lin1_enable();
								
								return INVERTER_COMM_SEQ6;
		
								break;

		case INVERTER_COMM_SEQ6:
								inverter_actual_comm_seq = INVERTER_COMM_SEQ6;

								board_pwm_hin1_disable();
								board_pwm_lin1_disable();

								board_pwm_lin3_disable();
								board_pwm_hin3_enable();

								board_pwm_hin2_disable();
								board_pwm_lin2_enable();

								return INVERTER_COMM_SEQ1;
		
								break;

		case INVERTER_COMM_FREWHEEL:
								inverter_actual_comm_seq = INVERTER_COMM_FREWHEEL;

								board_pwm_lin1_disable();
								board_pwm_lin2_disable();
								board_pwm_lin3_disable();
								board_pwm_hin1_disable();
								board_pwm_hin2_disable();
								board_pwm_hin3_disable();

								break;

		case INVERTER_COMM_BREAK_LOW:
								inverter_actual_comm_seq = INVERTER_COMM_BREAK_LOW;

								board_pwm_lin1_enable();
								board_pwm_lin2_enable();
								board_pwm_lin3_enable();
								board_pwm_hin1_disable();
								board_pwm_hin2_disable();
								board_pwm_hin3_disable();

								break;

		case INVERTER_COMM_BREAK_HIGH:
								inverter_actual_comm_seq = INVERTER_COMM_BREAK_HIGH;

								board_pwm_lin1_disable();
								board_pwm_lin2_disable();
								board_pwm_lin3_disable();
								board_pwm_hin1_enable();
								board_pwm_hin2_enable();
								board_pwm_hin3_enable();
								
								break;

		default:
							return ERROR;
	
	}
	return inverter_actual_comm_seq;
}


uint16_t SiguienteSeq (uint16_t seq)
{
	if(seq>=INVERTER_COMM_SEQ1 && seq<=INVERTER_COMM_SEQ6)
	{
		if(seq==INVERTER_COMM_SEQ6)
			seq = INVERTER_COMM_SEQ1;
		else
			seq<<=1;
		
		return seq;
	}
	else
		return -1;
	
}