#include "AL02_switches_leds_mux_driver.h"
#include "AL03_motor_3phase_driver.h"
#include "AL03_ui_interaction.h"


void ui_update (void)
{
    static int8_t	sw_command = 0;
    sw_command = ui_mux_get_switches_state();

    if (sw_command & (1<<UI_SWITCH_COLDSHOT_SHIFT_MASK))
    {

    }
    if (sw_command & (1<<UI_SWITCH_LOCK_SHIFT_MASK))
    {
        
    }
    if ( (sw_command & (1<<UI_SWITCH_INC_FAN_SHIFT_MASK)) != 0)
    {
        motor_3phase_speed_change(MORE_SPEED);
        //ui_leds_logic(MORE_SPEED);
    }
    if ( (sw_command & (1<<UI_SWITCH_DEC_FAN_SHIFT_MASK)) != 0)
    {
        motor_3phase_speed_change(LESS_SPEED);
        //ui_leds_logic(LESS_SPEED);
    }
    if ( (sw_command & (1<<UI_SWITCH_INC_HEAT_SHIFT_MASK)) != 0)
    {
        motor_3phase_phase_lead_change(MORE_LEAD);
        //ui_leds_logic(MORE_HEAT);
    }
    if ( (sw_command & (1<<UI_SWITCH_DEC_HEAT_SHIFT_MASK)) != 0)
    {
        motor_3phase_phase_lead_change(LESS_LEAD);
        //ui_leds_logic(LESS_HEAT);
    }

    
    //Reflejo los pulsadores en los LEDs
    ui_mux_set_leds_state(sw_command);

}


void ui_leds_logic (int32_t modif)
{
    uint8_t	led_aux = 0;
    uint8_t	led_speed = 0;
    uint8_t	led_heat = 0;

    led_aux = ui_mux_get_leds_state();

    led_speed = led_aux & SPEED_MASK;
    led_heat = led_aux & HEAT_MASK;

    if (modif == MORE_SPEED || modif == LESS_SPEED)
    {
        //Rama velocidad
        if (modif == MORE_SPEED)
        {
            if ( led_speed == 0b00001000 )
            {
                led_speed = 0b00011000;
            }
            else if ( led_speed == 0b00011000 )
            {
                led_speed = 0b00111000;
            }
            else
            {
                led_speed = 0b00111000;
            }
        }
        else
        {
            if ( led_speed == 0b00111000 )
            {
                led_speed = 0b00011000;
            }
            else if ( led_speed == 0b00011000 )
            {
                led_speed = 0b00001000;
            }
            else
            {
                led_speed = 0b00001000;
            }
        }
    }
    else
    {
        //Rama calor
        if (modif == MORE_HEAT)
        {
            if ( led_heat == 0b00000001 )
            {
                led_heat = 0b00000011;
            }
            else if ( led_heat == 0b00000011 )
            {
                led_heat = 0b00000111;
            }
            else
            {
                led_heat = 0b00000111;
            }
        }
        else
        {
            if ( led_heat == 0b00000111 )
            {
                led_heat = 0b00000011;
            }
            else if ( led_heat == 0b00000011 )
            {
                led_heat = 0b00000001;
            }
            else
            {
                led_heat = 0b00000001;
            }
        }
    }

    led_aux = led_speed | led_heat;
    ui_mux_set_leds_state(led_aux);
}