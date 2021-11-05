#include "AL02_switches_leds_mux_driver.h"


volatile static uint8_t switches_state=0,leds_state=0x00;

static bool switches_change_flag = FALSE;					//Flag de que los switches tienen un nuevo valor para revisar

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
 * Setea el valor de la variable leds. Para observar el cambio es necesario estar llamando a "ui_mux_leds_state_update"
 */
void 		ui_mux_set_leds_state			(uint8_t led_state)
{
	leds_state = led_state;
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


uint8_t read_switches_state (void)
{
	uint8_t switches_state;

	switches_state =	(ui_switch_coldshot_get_state()	<<	UI_SWITCH_COLDSHOT_SHIFT_MASK)  |
						(ui_switch_lock_get_state()		<<	UI_SWITCH_LOCK_SHIFT_MASK)		|
						(ui_switch_inc_fan_get_state()	<<	UI_SWITCH_INC_FAN_SHIFT_MASK)	|
						(ui_switch_dec_fan_get_state()	<<	UI_SWITCH_DEC_FAN_SHIFT_MASK)	|
						(ui_switch_inc_heat_get_state()	<<	UI_SWITCH_INC_HEAT_SHIFT_MASK)  |
						(ui_switch_dec_heat_get_state()	<<	UI_SWITCH_DEC_HEAT_SHIFT_MASK);

	return switches_state;
}


/* ----------- Codigo Jose ----------- */
void config_gpio_switch_pulldown (void)
{
	//Configura resistencias de pulldown en los pines de entrada
	GPIO_PullResistorConfig(BOARD_UI_IO_1_PORT, BOARD_UI_IO_1_PIN, GPIO_PR_DOWN);
	GPIO_PullResistorConfig(BOARD_UI_IO_2_PORT, BOARD_UI_IO_2_PIN, GPIO_PR_DOWN);
	GPIO_PullResistorConfig(BOARD_UI_IO_3_PORT, BOARD_UI_IO_3_PIN, GPIO_PR_DOWN);
	GPIO_PullResistorConfig(BOARD_UI_IO_4_PORT, BOARD_UI_IO_4_PIN, GPIO_PR_DOWN);
	GPIO_PullResistorConfig(BOARD_UI_IO_5_PORT, BOARD_UI_IO_5_PIN, GPIO_PR_DOWN);
	GPIO_PullResistorConfig(BOARD_UI_IO_6_PORT, BOARD_UI_IO_6_PIN, GPIO_PR_DOWN);
}

void config_gpio_switch_pullup (void)
{
	//Configura resistencias de pullup en los pines de entrada
	GPIO_PullResistorConfig(BOARD_UI_IO_1_PORT, BOARD_UI_IO_1_PIN, GPIO_PR_UP);
	GPIO_PullResistorConfig(BOARD_UI_IO_2_PORT, BOARD_UI_IO_2_PIN, GPIO_PR_UP);
	GPIO_PullResistorConfig(BOARD_UI_IO_3_PORT, BOARD_UI_IO_3_PIN, GPIO_PR_UP);
	GPIO_PullResistorConfig(BOARD_UI_IO_4_PORT, BOARD_UI_IO_4_PIN, GPIO_PR_UP);
	GPIO_PullResistorConfig(BOARD_UI_IO_5_PORT, BOARD_UI_IO_5_PIN, GPIO_PR_UP);
	GPIO_PullResistorConfig(BOARD_UI_IO_6_PORT, BOARD_UI_IO_6_PIN, GPIO_PR_UP);
}


void config_gpio_switch_pull_disable (void)
{
	//Configura resistencias de pullup en los pines de entrada
	GPIO_PullResistorConfig(BOARD_UI_IO_1_PORT, BOARD_UI_IO_1_PIN, GPIO_PR_DISABLE);
	GPIO_PullResistorConfig(BOARD_UI_IO_2_PORT, BOARD_UI_IO_2_PIN, GPIO_PR_DISABLE);
	GPIO_PullResistorConfig(BOARD_UI_IO_3_PORT, BOARD_UI_IO_3_PIN, GPIO_PR_DISABLE);
	GPIO_PullResistorConfig(BOARD_UI_IO_4_PORT, BOARD_UI_IO_4_PIN, GPIO_PR_DISABLE);
	GPIO_PullResistorConfig(BOARD_UI_IO_5_PORT, BOARD_UI_IO_5_PIN, GPIO_PR_DISABLE);
	GPIO_PullResistorConfig(BOARD_UI_IO_6_PORT, BOARD_UI_IO_6_PIN, GPIO_PR_DISABLE);
}


/*
 * Funcion que lee el estado de LEDs y realiza el cambio necesario
 * control: UI_FAN o UI_HEATER - Modifica los leds del Fan o el Calefactor
 * modificador: UI_UP o UI_DOWN - Enciende o apaga el LED correspondiente
 */
void ui_led_change (int8_t control, int8_t modificador)
{
	uint8_t led_aux;

	if (control == UI_FAN)
	{
		led_aux = leds_state & FAN_BIT_MASK;
		if ( modificador ==  UI_UP)
		{
			//Reviso si no estoy al maximo ya
			if (led_aux != 0b00111000)
			{
				//Desplazo para arriba y agrego un 1 nuevo al principio
				led_aux = (led_aux << 1) | 0b00001000;
			}
		}
		else
		{
			//Reviso si no estoy al minimo ya
			if (led_aux != 0b00001000)
			{
				//Desplazo para abajo y elimino los 1 restantes
				led_aux = (led_aux >> 1) & 0b00111000;
			}
		}

		//Defino el estado final
		led_aux = leds_state & (led_aux | 0b00000111);
	}
	else if ( control == UI_HEATER )
	{
		led_aux = leds_state & HEATER_BIT_MASK;
		if ( modificador ==  UI_UP)
		{
			//Reviso si no estoy al maximo ya
			if (led_aux != 0b00000111)
			{
				//Desplazo para arriba y agrego un 1 nuevo al principio
				led_aux = (led_aux << 1) | 0b00000001;
			}
		}
		else
		{
			//Reviso si no estoy al minimo ya
			if (led_aux != 0b00000001)
			{
				//Desplazo para abajo y elimino los 1 restantes
				led_aux = (led_aux >> 1) & 0b00111000;
			}
			else
			{
				//Puede estar totalmente apagado el calefactor
				led_aux = 0b00000000;
			}
		}

		//Defino el estado final
		led_aux = leds_state & (led_aux | 0b00111000);
	}
	
	ui_mux_set_leds_state(led_aux);
}