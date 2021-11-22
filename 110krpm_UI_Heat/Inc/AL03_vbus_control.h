#include "AL01_board_iq_aed_rev20191011.h"

#define VBUS_HIGH_LEVEL					1		//El minimo de la VBUS es el esperable con ROC (Rectificador Onda Completa)
#define VBUS_LOW_LEVEL					2		//El minimo de la VBUS es el esperable con RMO (Rectificador Media Onda)
#define VBUS_VERY_LOW_LEVEL				3		//El minimo de la VBUS es el esperable cuando se quito la alimentacion


#define VBUS_SAMPLE_READY_STATE			4		//Esperando analisis de muestras
#define VBUS_WAITING_STATE				5		//Esperando que se cumpla el timer

#define VBUS_CONFIG_STATE				10

// Estos valores se deben ajustar al motor y duty utilizado ya que influyen en el ripple / 3.3V - 4096
#define VBUS_HIGH_MIN_TH				2900		//Valor minimo de VBUS con ROC para pasar a alta velocidad - 2.392V medido
#define VBUS_LOW_MIN_TH					2850		//Valor minimo de VBUS con RMO para pasar a baja velocidad - 2.3V medido
#define VBUS_VERY_LOW_MIN_TH			2750		//Valor minimo de VBUS con RMO para pasar a baja velocidad - estimado


#define VBUS_SAMPLE_TIME_MS			7			//Tiempo entre 2 muestreos del VBUS

#define SAMPLE_WINDOW_SIZE			6			//Dimension del buffer de muestras del VBUS

#define UI_SPEED_CONTROL			0			//Indica que el control de velocidad es por UI
#define VBUS_SPEED_CONTROL			1			//Indica que el control de velocidad es por VBUS

void check_VBus_task (void);
int16_t min_value_buffer (int16_t *buffer);
int16_t max_value_buffer (int16_t *buffer);
int16_t avg_value_buffer (int16_t *buffer);
