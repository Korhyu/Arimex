#include "AL01_board_iq_aed_rev20191011.h"
#include "AL02_switches_leds_mux_driver.h"
#include "AL03_motor_3phase_driver.h"
#include "AL03_ui_interaction.h"

int32_t last_state=UI_STATE_LEDS;
int32_t actual_state=UI_STATE_STARTING;

void ui_task (void)
{
    int32_t ui_machine_state = 0;
    static int32_t ui_timer=0;
    static int32_t last_state=UI_STATE_STARTING;
    static int32_t actual_state=UI_STATE_LEDS;

    switch (actual_state)
    {
        case UI_STATE_STARTING:
            break;

        case UI_STATE_SWITCHES:	
            //Leo el estado de los switches y cambio al estado de LEDS
			
			//Leo el estado de los switches
			read_switches_state();
			
			//Pono los pines en el estado para manejar los LEDS
			change_state_to_leds();

            //Pongo el flag a la espera del vencimiento del timer
            //ui_set_actual_state(UI_WAITING_TIMER_LEDS);
            last_state = UI_STATE_SWITCHES;
            actual_state = UI_STATE_IDLE;

            break;

        case UI_STATE_LEDS:
            //Los LEDs se estan escribiendo por la funcion correspondiente, configuro los pines en el estado de SW
			
			//Pono los pines en el estado para leer los switches
			change_state_to_switches();

            //Pongo el flag a la espera del vencimiento del timer
            //ui_set_actual_state(UI_WAITING_TIMER_SWITCHES);
            last_state = UI_STATE_LEDS;
            actual_state = UI_STATE_IDLE;

            break;
        
        case UI_STATE_IDLE:
            if (ui_get_flag_state() == UI_TIMER_EXPIRED)
            {
                if (last_state == UI_STATE_LEDS)
                {
                    actual_state = UI_STATE_SWITCHES;
                    //En este estado leo los switches y luego dejo los LEDS listos
			        ui_timer_set_irq_within_us(MUX_TIME_LEDS_uS);		//Cargo el tiempo que quiero que este el estado de LEDS
                }
                else
                {
                    actual_state = UI_STATE_LEDS;
                    //En este estado leo los LEDs y luego dejo los SWs listos (hay una descarga de RC que tarda aprox 500us)
			        ui_timer_set_irq_within_us(MUX_TIME_SWITCHES_us);	//Cargo el tiempo que quiero leer los SW
                }
            }
            break;

        default:
            break;
    }

    if(ui_timer==0 || board_scheduler_is_time_expired(ui_timer))
    {
        ui_timer = board_scheduler_load_timer(TIME_REFRESH_mS);
        ui_update();
    }
}


void ui_set_actual_state (int32_t new_state)
{
    actual_state = new_state;
}

int32_t ui_get_actual_state (void)
{
   return actual_state;
}


void ui_update (void)
{
    static int8_t	sw_command = 0;
    static int32_t 	timer_ui = 0;		//Auxiliar para conteo de tiempo en UI

    sw_command = ui_mux_get_switches_state();

    if (sw_command & (1<<UI_SWITCH_COLDSHOT_SHIFT_MASK))
    {

    }
    if (sw_command & (1<<UI_SWITCH_LOCK_SHIFT_MASK))
    {
        
    }
    if ( (sw_command & (1<<UI_SWITCH_INC_FAN_SHIFT_MASK)) != 0)
    {
        //__hardware_gpio_output_set(GPIOB, 11);				//GPIO aux para monitoreo en OSC
        motor_3phase_speed_change(MORE_SPEED);
    }
    if ( (sw_command & (1<<UI_SWITCH_DEC_FAN_SHIFT_MASK)) != 0)
    {
        //__hardware_gpio_output_reset(GPIOB, 11);				//GPIO aux para monitoreo en OSC
        motor_3phase_speed_change(LESS_SPEED);
    }
    if ( (sw_command & (1<<UI_SWITCH_INC_HEAT_SHIFT_MASK)) != 0)
    {
        //__hardware_gpio_output_set(GPIOA, 3);					//GPIO aux para monitoreo en OSC
        motor_3phase_phase_lead_change(MORE_LEAD);
    }
    if ( (sw_command & (1<<UI_SWITCH_DEC_HEAT_SHIFT_MASK)) != 0)
    {
        //__hardware_gpio_output_reset(GPIOA, 3);				//GPIO aux para monitoreo en OSC
        motor_3phase_phase_lead_change(LESS_LEAD);
    }

    ui_mux_set_leds_state(sw_command);
    
    /*
    if(timer_ui==0 || board_scheduler_is_time_expired(timer_ui))
    {
        timer_ui = board_scheduler_load_timer(TIME_UI_mS);   
        if(var>=0x40)
            var=1;
        else
            var<<=1;
    }
    */
}