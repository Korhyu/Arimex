#include "AL02_switches_leds_mux_driver.h"
#include "AL03_motor_3phase_driver.h"
#include "AL03_ui_interaction.h"


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