
#include "stm32f0xx_hal.h"
#include "stdint.h"
#include "scheduler.h"
#include "hardware_functions_low_level.h"
#include "motor_trifase.h"

#define CONSOLE_TIME_TO_UPDATE_SCREEN_MS 		400
#define CONSOLE_TIME_TO_READ_UART_MS				100

#define CONSOLE_UPDATE_MODE_REFRESH_SCREEN  0
#define CONSOLE_UPDATE_MODE_PRINT_DOWN			1


void console_task (void);
void console_init(void);
