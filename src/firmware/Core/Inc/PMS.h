/**
 * @file    PMS.h
 * @brief   PMS5003 particulate matter sensor driver
 * @author  POS Meteo Station Team
 * @date    2025
 */

#ifndef INC_PMS_H_
#define INC_PMS_H_

#include "main.h"

/** @brief  Size of a complete PMS5003 data frame in bytes */
#define PMS_DATA_SIZE 32

/** @brief  Raw PMS5003 sensor frame (packed as received over UART) */
typedef struct __attribute__((packed)) {
	uint8_t start_1;               /**< Start char 1, 0x42 (fixed) */
	uint8_t start_2;               /**< Start char 2, 0x4D (fixed) */
	uint16_t frame_length;         /**< Frame length = 2x13 + 2 (data + parity) */

	uint16_t pm_1_0;               /**< PM1.0 CF=1 (ug/m3) */
	uint16_t pm_2_5;               /**< PM2.5 CF=1 (ug/m3) */
	uint16_t pm_10_0;              /**< PM10 CF=1 (ug/m3) */

	uint16_t pm_1_0_atmos;         /**< PM1.0 atmospheric (ug/m3) */
	uint16_t pm_2_5_atmos;         /**< PM2.5 atmospheric (ug/m3) */
	uint16_t pm_10_0_atmos;        /**< PM10 atmospheric (ug/m3) */

	uint16_t raw_gt_0_3;           /**< Particles >0.3um in 0.1L of air */
	uint16_t raw_gt_0_5;           /**< Particles >0.5um in 0.1L of air */
	uint16_t raw_gt_1_0;           /**< Particles >1.0um in 0.1L of air */
	uint16_t raw_gt_2_5;           /**< Particles >2.5um in 0.1L of air */
	uint16_t raw_gt_5_0;           /**< Particles >5.0um in 0.1L of air */
	uint16_t raw_gt_10_0;          /**< Particles >10um in 0.1L of air */

	uint8_t version_number;        /**< Firmware version */
	uint8_t error_code;            /**< Error code */

	uint16_t checksum;             /**< Frame checksum */
} PMS_Frame_t;

/** @brief  PMS5003 data access union (byte / word / frame views) */
typedef union {
	uint8_t bytes[PMS_DATA_SIZE];            /**< Raw byte view */
	uint16_t words[PMS_DATA_SIZE / 2];       /**< 16-bit word view */
	PMS_Frame_t frame;                       /**< Structured frame view */
} PMS_Data_t;

/** @brief  European Air Quality Index (EEA AQI) levels */
typedef enum {
	GOOD,             /**< 0 - Good */
	FAIR,             /**< 1 - Fair */
	MODERATE,         /**< 2 - Moderate */
	POOR,             /**< 3 - Poor */
	VERY_POOR,        /**< 4 - Very poor */
	EXTREMELY_POOR    /**< 5 - Extremely poor */
} European_Air_Quality_Index_t;

/**
 * @brief  Initialise the PMS5003 sensor (start DMA + enable power)
 * @param  pms_uart  Pointer to UART handle connected to the sensor
 * @retval HAL_OK on success, otherwise HAL_ERROR
 */
HAL_StatusTypeDef PMS_Init(UART_HandleTypeDef *pms_uart);

/**
 * @brief  Get the latest sensor data (converts from DMA buffer)
 * @return Pointer to a static PMS_Data_t with the latest frame
 */
PMS_Data_t* PMS_Get_Data();

/**
 * @brief  Validate a PMS5003 frame using its checksum
 * @param  data  Pointer to PMS data to validate
 * @return 1 if valid, 0 otherwise
 */
uint8_t PMS_Is_Data_Valid(PMS_Data_t *data);

/**
 * @brief  Calculate the European Air Quality Index from the data
 * @param  data  Pointer to PMS data with PM2.5 and PM10 values
 * @return Pointer to a static European_Air_Quality_Index_t
 */
European_Air_Quality_Index_t* PMS_Get_Quality_Index(PMS_Data_t *data);

#endif /* INC_PMS_H_ */
