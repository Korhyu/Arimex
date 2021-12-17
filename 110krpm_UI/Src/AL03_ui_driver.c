#include "AL03_ui_driver.h"
#include "AL03_motor_3phase_driver.h"


uint8_t switches_state;
int32_t ui_actual_state = UI_CONFIG_STATE;
int32_t ui_previous_state;


void ui_task (void)
{
	static uint32_t ui_timer = 0;
	switch (ui_actual_state)
	{
		case UI_WAITING_TIMER_STATE:

			if (board_scheduler_is_time_expired(ui_timer))
			{
				if (ui_previous_state == UI_LEDS_STATE)
				{
					ui_actual_state = UI_SWITCHES_STATE;
				}
				else if (ui_previous_state == UI_SWITCHES_STATE)
				{
					ui_actual_state = UI_LEDS_STATE;
				}
			}
			

			break;
		
		case UI_CONFIG_STATE:

			ui_mux_outputs_state_init();
			//ui_io_gpio_fun();
			ui_actual_state = UI_LEDS_STATE;
			ui_mux_set_leds_state(0b00111000);
			//board_timer_link_callback(ui_mux_callback,BOARD_TIM_UI_SEL_CALLBACK);
			ui_timer = board_scheduler_load_timer(200);

			break;

		case UI_LEDS_STATE:
			//Los LEDs se estan escribiendo por la funcion correspondiente, configuro los pines en el estado de SW

			//Pono los pines en el estado para leer los switches
			change_state_to_switches();

			//Pongo el flag a la espera del vencimiento del timer
			//ui_set_actual_state(UI_WAITING_TIMER_SWITCHES);
			ui_previous_state = UI_LEDS_STATE;
			ui_actual_state = UI_WAITING_TIMER_STATE;

			//ui_timer_set_irq_within_us(MUX_TIME_SWITCHES_us);
			ui_timer = board_scheduler_load_timer(2);

			break;

		case UI_SWITCHES_STATE:
			//Leo el estado de los switches y cambio al estado de LEDS

			//Leo el estado de los switches
			if ( get_switches_state() == UI_ACTION_REQUIRED )
			{
				//Analizo que SW se pulso y realizo la accion
				ui_update();

				//Si se pulso algun boton, puedo esperar un tiempo
				//ui_timer_set_irq_within_us(60000);				//Antirebote?
				ui_timer = board_scheduler_load_timer(198);
			}
			else
			{
				//ui_timer_set_irq_within_us(MUX_TIME_LEDS_uS);	//Tiempo normal
				ui_timer = board_scheduler_load_timer(8);
			}

			//Pono los pines en el estado para manejar los LEDS
			change_state_to_leds();

			//Pongo el flag a la espera del vencimiento del timer
			//ui_set_actual_state(UI_WAITING_TIMER_LEDS);
			ui_previous_state = UI_SWITCHES_STATE;
			ui_actual_state = UI_WAITING_TIMER_STATE;

			break;

		case UI_UPDATE_VARIABLES:
			//Si hay modificaciones en las variables del motor o calefactores las realizo aca

			

			break;
	
		default:
			break;
	}
}

/*
 * Funcion que lee el estado de los SW y los almacena en la variable global
 * y devuelve si es necesario realizar alguna modificacion en el estado
 */
int32_t get_switches_state (void)
{	
	switches_state = read_switches_state();

	if ( switches_state != 0b000000 )
	{
		//Se pulso algun SW y tengo que realizar alguna accion
		return UI_ACTION_REQUIRED;
	}
	else
	{
		//Si no se pulso ningun SW no hay que hacer modificacion
		return UI_NO_ACTION_REQUIRED;
	}
}


void change_state_to_leds (void)
{
	//Dejo todo listo para los LEDS
	ui_sw_reset_state();				//Pongo el pin alto del SW en reset para evitar que si se presiona se enciendan los LEDS
	ui_leds_reset_state();				//Pongo los pines bajos de los LEDS en reset para poder encenderlos
	ui_io_output_state();				//Configuro los pines medios como SALIDAS
}

void ui_mux_callback (void)
{
	//ui_timer_flag = UI_TIMER_EXPIRED;
	if (ui_previous_state == UI_LEDS_STATE)
	{
		ui_actual_state = UI_SWITCHES_STATE;
	}
	else if (ui_previous_state == UI_SWITCHES_STATE)
	{
		ui_actual_state = UI_LEDS_STATE;
	}
}

void change_state_to_switches (void)
{
	ui_sw_set_state();					//Pongo el pin alto del SW en set para leer el estado alto cuando se pulsa un pulsador
	ui_leds_set_state();				//Pongo los pines bajos de los LEDS en set para evitar que se enciendan cuando se pulsa un pulsador
	ui_io_intput_state();				//Configuro los pines medios como entradas.
}


void ui_update (void)
{
    static int8_t	sw_command = 0;
    static int32_t 	timer_ui = 0;		//Auxiliar para conteo de tiempo en UI

    sw_command = switches_state;

    if (switches_state & (1<<UI_SWITCH_COLDSHOT_SHIFT_MASK))
    {

    }
    if (switches_state & (1<<UI_SWITCH_LOCK_SHIFT_MASK))
    {
        
    }
    if ( (switches_state & (1<<UI_SWITCH_INC_FAN_SHIFT_MASK)) != 0)
    {
        //__hardware_gpio_output_set(GPIOB, 11);				//GPIO aux para monitoreo en OSC
        motor_3phase_speed_change(MORE_SPEED);
		ui_led_change(UI_FAN, UI_UP);
    }
    if ( (switches_state & (1<<UI_SWITCH_DEC_FAN_SHIFT_MASK)) != 0)
    {
        //__hardware_gpio_output_reset(GPIOB, 11);				//GPIO aux para monitoreo en OSC
        motor_3phase_speed_change(LESS_SPEED);
		ui_led_change(UI_FAN, UI_DOWN);
    }
    if ( (switches_state & (1<<UI_SWITCH_INC_HEAT_SHIFT_MASK)) != 0)
    {
        //__hardware_gpio_output_set(GPIOA, 3);					//GPIO aux para monitoreo en OSC
        motor_3phase_phase_lead_change(MORE_LEAD);
		ui_led_change(UI_HEATER, UI_UP);
    }
    if ( (switches_state & (1<<UI_SWITCH_DEC_HEAT_SHIFT_MASK)) != 0)
    {
        //__hardware_gpio_output_reset(GPIOA, 3);				//GPIO aux para monitoreo en OSC
        motor_3phase_phase_lead_change(LESS_LEAD);
		ui_led_change(UI_HEATER, UI_DOWN);
    }
}