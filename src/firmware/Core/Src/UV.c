#include "UV.h"

static UV_Config_t* UV_config_p;
static volatile  uint16_t adc_buff = 0;
static UV_Index_t UV_index;

HAL_StatusTypeDef UV_Init(UV_Config_t *config_p) {
	UV_config_p = config_p;
	return HAL_ADC_Start_DMA(UV_config_p->adc_p, (uint32_t*)(&adc_buff), 1);
}

UV_Index_t* UV_Get_Index(void) {
	uint16_t adc_voltage = (float)adc_buff / (float)UV_config_p->adc_max_value * UV_config_p->adc_max_voltage * 1000.0f;

	if (adc_voltage < UV_config_p->index_thresholds_mV[0]) {
		UV_index = UV_INDEX_0;
	} else if (adc_voltage < UV_config_p->index_thresholds_mV[1]) {
		UV_index = UV_INDEX_1;
	} else if (adc_voltage < UV_config_p->index_thresholds_mV[2]) {
		UV_index = UV_INDEX_2;
	} else if (adc_voltage < UV_config_p->index_thresholds_mV[3]) {
		UV_index = UV_INDEX_3;
	} else if (adc_voltage < UV_config_p->index_thresholds_mV[4]) {
		UV_index = UV_INDEX_4;
	} else if (adc_voltage < UV_config_p->index_thresholds_mV[5]) {
		UV_index = UV_INDEX_5;
	} else if (adc_voltage < UV_config_p->index_thresholds_mV[6]) {
		UV_index = UV_INDEX_6;
	} else if (adc_voltage < UV_config_p->index_thresholds_mV[7]) {
		UV_index = UV_INDEX_7;
	} else if (adc_voltage < UV_config_p->index_thresholds_mV[8]) {
		UV_index = UV_INDEX_8;
	} else if (adc_voltage < UV_config_p->index_thresholds_mV[9]) {
		UV_index = UV_INDEX_9;
	} else if (adc_voltage < UV_config_p->index_thresholds_mV[10]) {
		UV_index = UV_INDEX_10;
	} else {
		UV_index = UV_INDEX_11;
	}

	return &UV_index;
}
