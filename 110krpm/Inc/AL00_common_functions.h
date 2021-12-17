/*
 * AL00_common_functions.h
 *
 *  Created on: 5 ago. 2019
 *      Author: fgalli
 */

#ifndef AL00_COMMON_FUNCTIONS_H_
#define AL00_COMMON_FUNCTIONS_H_

//devuelve el valor actualizado al promedio o valor filtrado segun la ecuacion (Y(n) = (Y(n-1)*(k-1) + X(n))/k
#define common_update_average(average_value,new_sample,avg_factor)	(average_value=(((average_value<<avg_factor)-average_value+new_sample)>>avg_factor))



#endif /* AL00_COMMON_FUNCTIONS_H_ */
