/**
 * @file    UV.h
 * @brief   UV index sensor driver (ADC-based)
 * @author  POS Meteo Station Team
 * @date    2025
 */

#ifndef INC_UV_H_
#define INC_UV_H_
#include "main.h"

/** @brief  UV index levels (0-11) */
typedef enum {
	UV_INDEX_0,    /**< UV index 0 */
	UV_INDEX_1,    /**< UV index 1 */
	UV_INDEX_2,    /**< UV index 2 */
	UV_INDEX_3,    /**< UV index 3 */
	UV_INDEX_4,    /**< UV index 4 */
	UV_INDEX_5,    /**< UV index 5 */
	UV_INDEX_6,    /**< UV index 6 */
	UV_INDEX_7,    /**< UV index 7 */
	UV_INDEX_8,    /**< UV index 8 */
	UV_INDEX_9,    /**< UV index 9 */
	UV_INDEX_10,   /**< UV index 10 */
	UV_INDEX_11    /**< UV index 11 */
}UV_Index_t;

/** @brief  UV sensor configuration parameters */
typedef struct {
	ADC_HandleTypeDef* adc_p;          /**< ADC handle for reading the sensor */
	uint16_t adc_max_value;            /**< Maximum ADC reading (e.g. 4095 for 12-bit) */
	float adc_max_voltage;             /**< ADC reference voltage (e.g. 3.3) */
	uint16_t index_thresholds_mV[11];  /**< Thresholds in mV between UV index levels */
}UV_Config_t;

/**
 * @brief  Initialise the UV sensor (start ADC DMA)
 * @param  config  Pointer to UV configuration struct
 * @retval HAL_OK on success, otherwise HAL_ERROR
 */
HAL_StatusTypeDef UV_Init(UV_Config_t* config);

/**
 * @brief  Read the current UV index from the sensor
 * @return Pointer to a static UV_Index_t with the current level
 */
UV_Index_t* UV_Get_Index(void);

#endif /* INC_UV_H_ */
