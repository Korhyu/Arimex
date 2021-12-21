#include "AL01_board_iq_aed_rev20191011.h"

#define SC_VECTOR_SIZE		16				//Cantidad de muestras almacenadas en el vector (uso un 2**n porque me simplifica el promedio)
#define SC_SAMPLE_TIME_MS	50				//Tiempo entre muestras
#define SC_INITAL_WAIT		500				//Espera inicial antes de comenzar a tomar muestras (TODO: hay que ver cuanto demora el arranque para ponerlo cuando termina el mismo)

#define	SC_CONFIG_STATE		0				//Estado de confifuracion
#define	SC_WORKING_STATE	1				//Estado de toma de muestra y evaluacion de velocidad
#define	SC_DECISION_STATE	2				//Tengo los datos necesarios para tomar una decision

#define	SC_IN_RANGE     	0				//La velocidad esta dentro del rango especificado
#define	SC_ABOVE_RANGE  	1				//La velocidad esta por encima del rango especificado
#define	SC_BELOW_RANGE  	2				//La velocidad esta por debajo del rango especificado


void	sc_task			(void);					//Tarea que toma las muestras y evalua si tomar acciones o no
int32_t	sc_sample		(void);	    		    //Devuelve el valor de velocidad solicitado al driver de motor la velocidad actual
int32_t	sc_average		(int32_t*);		        //Devuelve el promedio del vector de muestras de velocidad
int32_t	sc_ask_sp		(void);			        //Solicita el set point de velocidad
int32_t sc_delta_calc   (int32_t, int32_t);		//Calcula el vlor absoluto de variacion especificada del parametro pasado 
