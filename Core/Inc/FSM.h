#ifndef INC_FSM_H_
#define INC_FSM_H_

#include "main.h"
#include "BME280_STM32.h"
#include "UV.h"
#include "gps.h"
#include "PMS.h"
#include "CC1101_BSP.h"
#include <stdio.h>

typedef enum {
	IDLE,
	WAKEUP,
	MEASURE,
	SYNC_TIME,
	PROCESS_DATA,
	TX,
	RX,

}FSM_State_t;


void FSM_Run(void);

uint8_t Init(UV_Config_t* uv_config_p);
uint8_t modules_sleep(void);
uint8_t modules_wakeup(void);
void Debug_Radio_Data(UART_HandleTypeDef *huart, Radio_Data_t *data);
void RTC_set_time(uint8_t h, uint8_t m, uint8_t s);
void RTC_set_date(uint8_t weekday, uint8_t d,  uint8_t m, uint8_t y);
void RTC_get_time_date(RTC_DateTypeDef* date_p, RTC_TimeTypeDef* time_p);

#endif /* INC_FSM_H_ */
