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

static uint8_t Init(UV_Config_t* uv_config_p);
static uint8_t modules_sleep(void);
static uint8_t modules_wakeup(void);

#endif /* INC_FSM_H_ */
