#ifndef AL02_SWITCHES_LEDS_MUX_DRIVER_H

#define AL02_SWITCHES_LEDS_MUX_DRIVER_H

#include "AL01_board_iq_aed_rev20191011.h"

#define UI_SWITCH_COLDSHOT_SHIFT_MASK		5
#define UI_SWITCH_LOCK_SHIFT_MASK				4
#define UI_SWITCH_INC_FAN_SHIFT_MASK		3
#define UI_SWITCH_DEC_FAN_SHIFT_MASK		2
#define UI_SWITCH_INC_HEAT_SHIFT_MASK		1
#define UI_SWITCH_DEC_HEAT_SHIFT_MASK		0

#define UI_LED_MIN_HEAT_SHIFT_MASK			0
#define UI_LED_MID_HEAT_SHIFT_MASK			1
#define UI_LED_MAX_HEAT_SHIFT_MASK			2
#define UI_LED_MIN_FAN_SHIFT_MASK				3
#define UI_LED_MID_FAN_SHIFT_MASK				4
#define UI_LED_MAX_FAN_SHIFT_MASK				5


uint8_t ui_mux_get_switches_state (void);
void 		ui_mux_set_leds_state			(uint8_t led_state);
uint8_t ui_mux_get_leds_state			(void);
void 		ui_mux_driver_routine 		(void);
void		ui_mux_leds_state_update 	(void);
void 		ui_mux_init 							(void);

#endif /* AL02_SWITCHES_LEDS_MUX_DRIVER_H */
