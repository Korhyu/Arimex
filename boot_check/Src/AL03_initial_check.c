#include "AL03_initial_check.h"
#include "AL03_motor_3phase_driver.h"

int ic_task(void)
{
	static uint32_t status;
	int32_t check_step = IC_VOL_CHECK;
	int32_t ic_timer = 300;

	if(board_scheduler_is_time_expired(ic_timer))
	{
		switch (check_step)
		{
			case IC_VOL_CHECK:
				//Control de sobre y sub tension
				status = ic_voltage_check();
				if (status == IC_MEASURING)
				{
					ic_timer = board_scheduler_load_timer(1);
				}
				else if (status == IC_OK)
				{
					
					check_step = IC_NTC_CHECK;
					//ic_timer = board_scheduler_load_timer(1);
				}
				else if (status == IC_OVERV)
				{
					//Sobretension
				}
				else if (status == IC_UNDERV)
				{
					//Subtension
				}

				break;

			case IC_NTC_CHECK:
				//Control de NTCs en rango
				if (ic_ntc_check() == IC_OK)
				{
					check_step = IC_SEQ_CHECK;
				}
				else
				{
					//Problema de sobretemperatura
				}

				break;

			case IC_SEQ_CHECK:
				//Exitacion de fases para control de las mismas
				
				break;
			
			default:
				break;
		}
	}
}

int ic_voltage_check (void)
{
	uint16_t vbus_sample[SAMPLES];
	uint16_t max = 0;				//Inicializo con los valores imposibles
	uint16_t min = 0xFFFF;
	static uint16_t index = 0;

	if (index < SAMPLES)
	{
		vbus_sample[index] = board_adc_get_vbus_voltage_C();
		index++;

		return IC_MEASURING;
	}
	else
	{
		for (index=0 ; index < SAMPLES ; index++)
		{
			if(vbus_sample[index] > max)
			{
				max = vbus_sample[index];
			}
			if(vbus_sample[index] < min)
			{
				min = vbus_sample[index];
			}
		}

		if(max >= MAX_VOL_VBUS)
		{
			//Sobretension
			return IC_OVERV;
		}
		else if(min <= MIN_VOL_VBUS)
		{
			//Subtension
			return IC_UNDERV;
		}
		else
		{
			//Puedo continuar
			return IC_OK;
		}
	}
}


int ic_ntc_check ()
{
	if ( board_adc_get_power_drivers_temp_C() >= MAX_TEMP_DRIVER)
	{
		//Sobretemperatura en el driver
		return IC_NTCD_ERROR;
	}
	else if ( board_adc_get_heater_temp_C() >= MAX_TEMP_HEATER)
	{
		//Sobretemperatura en el heater
		return IC_NTCH_ERROR;
	}
	else
	{
		return IC_OK
	}
}