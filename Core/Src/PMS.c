#include "PMS.h"
#include <string.h>

static volatile uint8_t pms_data[PMS_DATA_SIZE] = { 0 };
static PMS_Data_t PMS_Data = { 0 };
static European_Air_Quality_Index_t PMS_Air_Quality_Index;

HAL_StatusTypeDef PMS_Init(UART_HandleTypeDef *pms_uart) {
	return HAL_UART_Receive_DMA(pms_uart, (uint8_t*) pms_data, PMS_DATA_SIZE);
}

PMS_Data_t* PMS_Get_Data() {
	memcpy(PMS_Data.bytes, (uint8_t*) pms_data, PMS_DATA_SIZE);

	uint8_t word = 1;

	for (uint8_t i = 0; i < 13; i++) {
		PMS_Data.words[word] = (PMS_Data.bytes[word * 2] << 8) | PMS_Data.bytes[word * 2 + 1];
		word++;
	}
	word++; // skip version and error code bytes
	PMS_Data.words[word] = (PMS_Data.bytes[word * 2] << 8) | PMS_Data.bytes[word * 2 + 1];

	return &PMS_Data;
}

uint8_t PMS_Is_Data_Valid(PMS_Data_t *data) {
	uint16_t sum = 0;

	for (uint8_t i = 0; i < (PMS_DATA_SIZE - 2); i++) { // -2 to skip CRC bytes
		sum += data->bytes[i];
	}

	return (sum == data->frame.checksum) ? 1 : 0;
}

European_Air_Quality_Index_t* PMS_Get_Quality_Index(PMS_Data_t *data) {
	if (data->frame.pm_2_5_atmos < 6 && data->frame.pm_10_0_atmos < 16) {
		PMS_Air_Quality_Index = GOOD;
	} else if (data->frame.pm_2_5_atmos < 16 && data->frame.pm_10_0_atmos < 46) {
		PMS_Air_Quality_Index = FAIR;
	} else if (data->frame.pm_2_5_atmos < 51 && data->frame.pm_10_0_atmos < 121) {
		PMS_Air_Quality_Index = MODERATE;
	} else if (data->frame.pm_2_5_atmos < 91 && data->frame.pm_10_0_atmos < 196) {
		PMS_Air_Quality_Index = POOR;
	} else if (data->frame.pm_2_5_atmos < 140 && data->frame.pm_10_0_atmos < 270) {
		PMS_Air_Quality_Index = VERY_POOR;
	} else {
		PMS_Air_Quality_Index = EXTREMELY_POOR;
	}

	return &PMS_Air_Quality_Index;
}

