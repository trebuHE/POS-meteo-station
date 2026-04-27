#ifndef INC_UV_H_
#define INC_UV_H_
#include "main.h"

typedef enum {
	UV_INDEX_0,
	UV_INDEX_1,
	UV_INDEX_2,
	UV_INDEX_3,
	UV_INDEX_4,
	UV_INDEX_5,
	UV_INDEX_6,
	UV_INDEX_7,
	UV_INDEX_8,
	UV_INDEX_9,
	UV_INDEX_10,
	UV_INDEX_11
}UV_Index_t;

typedef struct {
	ADC_HandleTypeDef* adc_p;
	uint16_t adc_max_value;
	float adc_max_voltage;
	uint16_t index_thresholds_mV[11];
}UV_Config_t;

HAL_StatusTypeDef UV_Init(UV_Config_t* config);
UV_Index_t* UV_Get_Index(void);

#endif /* INC_UV_H_ */
