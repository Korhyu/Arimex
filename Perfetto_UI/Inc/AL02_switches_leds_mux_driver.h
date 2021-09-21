#ifndef AL02_SWITCHES_LEDS_MUX_DRIVER_H

#define AL02_SWITCHES_LEDS_MUX_DRIVER_H

#include "AL01_board_iq_aed_rev20191011.h"

#define UI_SWITCH_COLDSHOT_SHIFT_MASK		5
#define UI_SWITCH_LOCK_SHIFT_MASK			4
#define UI_SWITCH_INC_FAN_SHIFT_MASK		3
#define UI_SWITCH_DEC_FAN_SHIFT_MASK		2
#define UI_SWITCH_INC_HEAT_SHIFT_MASK		1
#define UI_SWITCH_DEC_HEAT_SHIFT_MASK		0

#define UI_LED_MIN_HEAT_SHIFT_MASK			0
#define UI_LED_MID_HEAT_SHIFT_MASK			1
#define UI_LED_MAX_HEAT_SHIFT_MASK			2
#define UI_LED_MIN_FAN_SHIFT_MASK			3
#define UI_LED_MID_FAN_SHIFT_MASK			4
#define UI_LED_MAX_FAN_SHIFT_MASK			5


//Acciones de los switches
#define NO_CHANGE_SW					0              //Sin cambios respecto al estado anterior
#define COLDSHOT_ON_SW 				    1
#define COLDSHOT_OFF_SW	    			2
#define LOCK_ON_SW						3
#define LOCK_OFF_SW			    		4
#define MOTOR_UP_SW				    	5
#define MOTOR_DOWN_SW					6
#define HEAT_UP_SW						7
#define HEAT_DOWN_SW					8


uint8_t     ui_mux_get_switches_state   (void);
void 		ui_mux_set_leds_state		(uint8_t led_state);
uint8_t     ui_mux_get_leds_state		(void);
void 		ui_mux_driver_routine 		(void);
void		ui_mux_leds_state_update 	(void);
void 		ui_mux_init 				(void);

uint8_t     switches_state_changed          (void);
void        reset_switches_state_changed    (void);
uint8_t     switches_status                 (void);                //Devuelve si se opero alguna accion con la UI
void        config_gpio_switch_pulldown     (void);
void        config_gpio_switch_pullup       (void);
void        config_gpio_switch_pull_disable (void);

#endif /* AL02_SWITCHES_LEDS_MUX_DRIVER_H */
