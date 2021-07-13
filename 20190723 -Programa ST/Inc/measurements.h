#ifndef __MEASUREMENTS_H

#define __MEASUREMENTS_H

#include "stm32f0xx_hal.h"
#include "stdint.h"
#include "hardware_functions_low_level.h"
#include "scheduler.h"
#include "motor_trifase.h"
#define MEASUREMENTS_SAMPLE_TIME_MS							1



void measurements_task (void);
void measurements_init(void);



#endif /* __MOTOR_TRIFASE_H */

