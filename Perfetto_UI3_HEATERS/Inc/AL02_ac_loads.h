#include "AL01_board_iq_aed_rev20191011.h"

// Salida Opto 1 puerto A pin 3
// Salida Opto 2 puerto B pin 11
// Entrada ZCD_SENSE puerto A pin 14


#define HEATER_ON   0       //Calentador encendido
#define HEATER_OFF  1       //Calentador apagado

#define HEATER_1    0       //Calentador 1
#define HEATER_2    1       //Calentador 2

#define VBUS_LOW_STATE    0       //VBUS esta por debajo del umbral de deteccion
#define VBUS_HIGH_STATE   1       //VBUS esta por encima del umbral de deteccion

#define TIME_HEATER_REFRESH_mS      100         //Cada 100ms reactivo el control, esta va a ser resolucion de tiempo

#define HEATER_COUNTER_RESET        10          //Contador de TIME_HEATER_REFRESH_mS, cuando llegue a esta cantidad reinicia el conteo (Periodo del PWM)

//Macros para activar o apagar los diferentes canales calefactores
#define board_heater_1_disable()						__hardware_gpio_output_reset(BOARD_HEATER_1_PORT,BOARD_HEATER_1_PIN)
#define board_heater_1_enable() 						__hardware_gpio_output_set(BOARD_HEATER_1_PORT,BOARD_HEATER_1_PIN)
#define board_heater_2_disable()						__hardware_gpio_output_reset(BOARD_HEATER_2_PORT,BOARD_HEATER_2_PIN)
#define board_heater_2_enable()    					__hardware_gpio_output_set(BOARD_HEATER_2_PORT,BOARD_HEATER_2_PIN)

//Macros para la lectura del pin de zcd_detect
#define board_vbus_zcd_get_state()                      __hardware_gpio_input_read_state(BOARD_ZCD_LINE_PORT,BOARD_ZCD_LINE_PIN)

void    heater_task         (void);                                     //Maquina de estados que controla el calefactor
int32_t heater_zcd_vbus     (void);                                     //Indica si vbus esta en 0V (aproximadamente)
void    heater_command      (int32_t heater, int32_t new_state);        //Funcion que enciende o apaga el calentador indicado

int32_t heater_get_duty     (void);                                     //Devuelve el duty aproximado actual (heater_counter_threshold / HEATER_COUNTER_RESET) * 100
void    heater_set_duty     (int32_t duty);                             //Configura el duty aproximado (heater_counter_threshold / HEATER_COUNTER_RESET) * 100
