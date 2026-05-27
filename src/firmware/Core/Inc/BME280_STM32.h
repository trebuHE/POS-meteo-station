/**
 * @file    BME280_STM32.h
 * @brief   BME280 temperature, pressure and humidity sensor driver
 * @author  ControllersTech.com
 * @date    Dec 14, 2021
 *
 * @copyright Copyright (C) 2017 ControllersTech.com
 * This is a free software under the GNU license, you can redistribute it and/or modify it under the terms
 * of the GNU General Public License version 3 as published by the Free Software Foundation.
 * This software library is shared with public for educational purposes, without WARRANTY and Author is
 * not liable for any damages caused directly or indirectly by this software, read more about this on
 * the GNU General Public License.
 */


#ifndef INC_BME280_STM32_H_
#define INC_BME280_STM32_H_

#include "main.h"

#define BME280_I2C hi2c1

#define SUPPORT_64BIT 1
//#define SUPPORT_32BIT 1

#define BME280_ADDRESS 0xEC  // SDIO is grounded, the 7 bit address is 0x76 and 8 bit address = 0x76<<1 = 0xEC


extern I2C_HandleTypeDef BME280_I2C;


/** @brief BME280 measurement data container */
typedef struct {
	float temperature; /**< Temperature in degrees Celsius */
	float pressure;    /**< Pressure in Pascals */
	float humidity;    /**< Humidity in %RH */
}BME280_Data_t;


/**
 * @brief  Configure the BME280 sensor
 * @param  osrs_t  Temperature oversampling (OSRS_OFF .. OSRS_16)
 * @param  osrs_p  Pressure oversampling (OSRS_OFF .. OSRS_16)
 * @param  osrs_h  Humidity oversampling (OSRS_OFF .. OSRS_16)
 * @param  mode    Sensor mode (MODE_SLEEP, MODE_FORCED, MODE_NORMAL)
 * @param  t_sb    Standby time (T_SB_0p5 .. T_SB_20)
 * @param  filter  IIR filter coefficient (IIR_OFF .. IIR_16)
 * @retval 0 on success, non-zero error code on failure
 */
int BME280_Config (uint8_t osrs_t, uint8_t osrs_p, uint8_t osrs_h, uint8_t mode, uint8_t t_sb, uint8_t filter);

/**
 * @brief  Wake the sensor from sleep for a forced measurement
 */
void BME280_WakeUP(void);

/**
 * @brief  Perform a measurement and retrieve temperature, pressure, humidity
 * @param  temperature  Pointer to store temperature (°C)
 * @param  pressure     Pointer to store pressure (Pa)
 * @param  humidity     Pointer to store humidity (%RH)
 */
void BME280_Measure(float *temperature, float *pressure, float *humidity);

/**
 * @brief  Initialise the BME280 with default settings
 * @retval 0 on success, non-zero on failure
 */
int BME280_Init(void);

/**
 * @brief  Perform a full measurement cycle and return the data
 * @return Pointer to a static BME280_Data_t with latest values
 */
BME280_Data_t* BME280_Get_Data(void);


/** Oversampling definitions */
#define OSRS_OFF    	0x00 /**< No oversampling (measurement skipped) */
#define OSRS_1      	0x01 /**< Oversampling x1 */
#define OSRS_2      	0x02 /**< Oversampling x2 */
#define OSRS_4      	0x03 /**< Oversampling x4 */
#define OSRS_8      	0x04 /**< Oversampling x8 */
#define OSRS_16     	0x05 /**< Oversampling x16 */

/** Mode definitions */
#define MODE_SLEEP      0x00 /**< Sleep mode */
#define MODE_FORCED     0x01 /**< Forced mode (single measurement, then sleep) */
#define MODE_NORMAL     0x03 /**< Normal continuous mode */

/** Standby time (normal mode) */
#define T_SB_0p5    	0x00 /**< Standby 0.5 ms */
#define T_SB_62p5   	0x01 /**< Standby 62.5 ms */
#define T_SB_125    	0x02 /**< Standby 125 ms */
#define T_SB_250    	0x03 /**< Standby 250 ms */
#define T_SB_500    	0x04 /**< Standby 500 ms */
#define T_SB_1000   	0x05 /**< Standby 1000 ms */
#define T_SB_10     	0x06 /**< Standby 10 ms */
#define T_SB_20     	0x07 /**< Standby 20 ms */

/** IIR filter coefficients */
#define IIR_OFF     	0x00 /**< Filter off */
#define IIR_2       	0x01 /**< Filter coefficient 2 */
#define IIR_4       	0x02 /**< Filter coefficient 4 */
#define IIR_8       	0x03 /**< Filter coefficient 8 */
#define IIR_16      	0x04 /**< Filter coefficient 16 */

/** Register definitions */
#define ID_REG      	0xD0 /**< Chip ID register */
#define RESET_REG  		0xE0 /**< Soft reset register */
#define CTRL_HUM_REG    0xF2 /**< Humidity control register */
#define STATUS_REG      0xF3 /**< Status register */
#define CTRL_MEAS_REG   0xF4 /**< Measurement control register */
#define CONFIG_REG      0xF5 /**< Configuration register */
#define PRESS_MSB_REG   0xF7 /**< Pressure MSB register */


#endif /* INC_BME280_STM32_H_ */
