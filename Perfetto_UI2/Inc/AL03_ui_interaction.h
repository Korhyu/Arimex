

#define TIME_UI_mS		1000			//Tiempo entre verificaciones de cambios de la UI
#define TIME_REFRESH_mS	200 			//Tiempo entre lecturas de la UI

//Estados de la maquina de estados
#define UI_STATE_RESET				0
#define UI_STATE_LEDS				1
#define UI_STATE_SWITCHES			2
#define UI_WAITING_TIMER			3
#define UI_WAITING_TIMER_LEDS       4
#define UI_WAITING_TIMER_SWITCHES   5
#define UI_STATE_IDLE               6
#define UI_STATE_STARTING           7

void	ui_update	(void);
void	ui_task	    (void);
void    ui_set_actual_state (int32_t);
int32_t ui_get_actual_state (void);