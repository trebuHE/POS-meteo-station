#ifndef INC_PMS_H_
#define INC_PMS_H_

#include "main.h"

#define PMS_DATA_SIZE 32

typedef struct __attribute__((packed)) {
	uint8_t start_1;          // Start char 1 0x42 (fixed)
	uint8_t start_2;          // Start char 2 0x4d (fixed)
	uint16_t frame_length;    // Frame length = 2x13 + 2 (data + parity)

	uint16_t pm_1_0;          // PM1.0 concentration (CF = 1, standard particles) Unit ug/m^3
	uint16_t pm_2_5;          // PM2.5 concentration (CF = 1, standard particulates) Unit ug/m^3
	uint16_t pm_10_0;         // PM10 concentration (CF = 1, standard particulate matter) Unit ug/m^3

	uint16_t pm_1_0_atmos;    // PM1.0 concentration (in the atmosphere) Unit ug/m^3
	uint16_t pm_2_5_atmos;    // PM2.5 concentration (in the atmosphere) Unit ug/m^3
	uint16_t pm_10_0_atmos;   // PM10 concentration (in the atmosphere) Unit ug/m^3

	uint16_t raw_gt_0_3;      // Particles in 0.1 liter of air > 0.3um
	uint16_t raw_gt_0_5;      // Particles in 0.1 liter of air > 0.5um
	uint16_t raw_gt_1_0;      // Particles in 0.1 liter of air > 1.0um
	uint16_t raw_gt_2_5;      // Particles in 0.1 liter of air > 2.5um
	uint16_t raw_gt_5_0;      // Particles in 0.1 liter of air > 5.0um
	uint16_t raw_gt_10_0;     // Particles in 0.1 liter of air > 10um

	uint8_t version_number;
	uint8_t error_code;

	uint16_t checksum;
} PMS_Frame_t;

typedef union {
	uint8_t bytes[PMS_DATA_SIZE];
	uint16_t words[PMS_DATA_SIZE / 2];
	PMS_Frame_t frame;
} PMS_Data_t;

typedef enum { // https://airindex.eea.europa.eu/AQI/
	GOOD,
	FAIR,
	MODERATE,
	POOR,
	VERY_POOR,
	EXTREMELY_POOR
} European_Air_Quality_Index_t;

HAL_StatusTypeDef PMS_Init(UART_HandleTypeDef *pms_uart);
PMS_Data_t* PMS_Get_Data();
uint8_t PMS_Is_Data_Valid(PMS_Data_t *data);
European_Air_Quality_Index_t* PMS_Get_Quality_Index(PMS_Data_t *data);
#endif /* INC_PMS_H_ */
