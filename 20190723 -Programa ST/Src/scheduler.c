#include "scheduler.h"

//Calcula el tiempo futuro en el que debe ejecutarse una tarea 
//(calcula a partir de que Numero de tick debe ejecutar la tarea)
int32_t scheduler_next_time_exec (int32_t period_of_exec)
{
		int32_t actual_tick;
		actual_tick=HAL_GetTick();
		actual_tick+=period_of_exec;
		return actual_tick;
}


//Revisa si debe ejecutarse o no una tarea, en funcion del Numero de tick actual 
//y el numero de ticks pasado por parametro
int32_t scheduler_is_time_to_exec (int32_t time_for_exec)
{
		int32_t actual_tick;
		actual_tick=HAL_GetTick();
		actual_tick-=time_for_exec;
	
		if(actual_tick>=0)
			return 1;
		else
			return 0;
}

