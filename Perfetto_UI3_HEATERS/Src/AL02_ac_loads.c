#include "AL02_ac_loads.h"


/*
Hay que definir los pines de entrada y salida
La idea es que las velas del calefactor trabajen un porcentaje indicado por una variable
Hay que habilitar la conduccion de los tiristores cuando se detecta 0V en el pin de entrada
Y cortar la conduccion tambien en el 0V en el mismo pin
La conduccion debera estar encendida durante X cantidad de ciclos de acuerdo a lo indicado en la variable
Funcionara como un DUTY pero el periodo seria constante y seria la cantidad de ciclos totales, sobre eso
serian la cantidad de ciclos activos.

*/

//board_comp_bemf_rising_link_callback(heater_control,BOARD_COMP_BEMF_U_PHASE_SELECT);      //Alternativa controlada por EXTI

int32_t heater_counter_threshold = 0;

void heater_task (void)
{
    static int32_t 	timer_heater = 0;		//Auxiliar para conteo de tiempo en UI
    static int32_t  heater_zcd_count = 0;   //Contador de zcds cada TIME_HEATER_REFRESH_mS (Tener en cuenta para el conteo de tiempos)
	
    //Reviso si el tiempo se cumplio
	if(timer_heater==0 || board_scheduler_is_time_expired(timer_heater))
	{
        if (heater_zcd_vbus() == VBUS_LOW_STATE)
        {
            //Recargo el timer
            timer_heater = board_scheduler_load_timer(TIME_HEATER_REFRESH_mS);

            //Aumento el contador
            heater_zcd_count++;

            //Reviso que no supere el maximo
            heater_zcd_count %= HEATER_COUNTER_RESET;
        }

        if (heater_counter_threshold == HEATER_COUNTER_RESET)
        {
            //100% de duty
            board_heater_1_enable();
            board_heater_2_enable();
        }
        else if (heater_counter_threshold == 0)
        {
            //0% de duty
            board_heater_1_disable();
            board_heater_2_disable();
        }
        else if (heater_zcd_count >= heater_counter_threshold)
        {
            //Estoy por encima del umbral de apagado (estoy en toff)
            board_heater_1_disable();
            board_heater_2_disable();
        }
        else
        {
            //Estoy por debajo del umbral de apagado (estoy en ton)
            board_heater_1_enable();
            board_heater_2_enable();
        }
	}  
}

int32_t heater_zcd_vbus (void)
{
    if (board_vbus_zcd_get_state() == 0)
    {
        return VBUS_LOW_STATE;
    }
    else
    {
        return VBUS_HIGH_STATE;
    }
}


void heater_set_duty (int32_t duty)
{
    heater_counter_threshold = (duty * HEATER_COUNTER_RESET) / 100;
}

int32_t heater_get_duty (void)
{
    return ((heater_counter_threshold * 100) / HEATER_COUNTER_RESET);
}


//No la termine usando esta funcion
void heater_command (int32_t heater, int32_t new_state)
{
    if (new_state == HEATER_ON)
    {
        if (heater == HEATER_1)
        {
            board_heater_1_enable();
        }
        else
        {
            board_heater_2_enable();
        }
    }
    else
    {
        if (heater == HEATER_1)
        {
            board_heater_1_disable();
        }
        else
        {
            board_heater_2_disable();
        }
    }
}