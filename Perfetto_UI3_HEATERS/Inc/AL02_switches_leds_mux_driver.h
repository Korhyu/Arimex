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


#define MUX_TIME_LEDS_uS			49500
#define MUX_TIME_SWITCHES_us	 	2000
#define TIME_REFRESH_mS	            250 			//Tiempo entre lecturas de la UI

#define UI_STATE_RESET				0
#define UI_STATE_LEDS				1
#define UI_STATE_SWITCHES			2

#define UI_TIMER_EXPIRED			0
#define UI_TIMER_RUNNING			1


uint8_t     ui_mux_get_switches_state   (void);
uint8_t     ui_mux_get_leds_state		(void);
void 		ui_mux_driver_routine 		(void);
void		ui_mux_leds_state_update 	(void);
void 		ui_mux_init 				(void);
void 		ui_mux_set_leds_state		(uint8_t);


uint8_t     switches_status             (void);                //Devuelve si se opero alguna accion con la UI
uint8_t     switches_state_changed      (void);
void        config_gpio_switch_pulldown (void);
void        config_gpio_switch_pullup   (void);
void        reset_switches_state_changed (void);
void        config_gpio_switch_pull_disable   (void);


void        ui_timer_callback           (void);
int8_t      ui_timer_get_status        	(void);
void        ui_timer_set_status         (int8_t);
void        ui_task                     (void);
void        ui_update                   (void);

#endif /* AL02_SWITCHES_LEDS_MUX_DRIVER_H */
