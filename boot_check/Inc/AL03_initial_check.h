#define MAX_VOL_VBUS	3200		//Valor a verificar
#define MIN_VOL_VBUS	2200		//Valor a verificar

#define MAX_TEMP_DRIVER	3200		//Valor a verificar
#define MAX_TEMP_HEATER	3200		//Valor a verificar

#define IC_VOL_CHECK	1
#define IC_NTC_CHECK	2
#define IC_SEQ_CHECK	3

#define IC_VOL_ERROR	11
#define IC_NTCD_ERROR	12
#define IC_NTCH_ERROR	13
#define IC_MOT_ERROR	14


#define IC_OK				0			//OK
#define IC_OVERV			1			//Over Voltage
#define IC_UNDERV			2			//Under Voltage
#define IC_OVER_TEMP_D		3			//Sobretemperatura en el driver
#define IC_OVER_TEMP_H		4			//Sobretemperatura en el heater

#define	IC_MEASURING	9			//Tomando medicoiones
#define IC_START		10			//Valor inicial


#define SAMPLES			30
