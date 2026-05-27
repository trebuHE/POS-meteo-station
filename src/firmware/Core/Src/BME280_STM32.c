/**
 * @file    BME280_STM32.c
 * @brief   BME280 sensor driver implementation
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

#include "BME280_STM32.h"

static int TrimRead(void);
static int BMEReadRaw(void);
static int32_t BME280_compensate_T_int32(int32_t adc_T);
static uint32_t BME280_compensate_P_int64(int32_t adc_P);
//static uint32_t BME280_compensate_P_int32(int32_t adc_P);
static uint32_t bme280_compensate_H_int32(int32_t adc_H);

uint8_t TrimParam[36];
int32_t tRaw, pRaw, hRaw;

uint16_t dig_T1,  \
         dig_P1, \
         dig_H1, dig_H3;

int16_t  dig_T2, dig_T3, \
         dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9, \
		 dig_H2,  dig_H4, dig_H5, dig_H6;

int32_t t_fine;
uint8_t chipID;

static BME280_Data_t data = {0};


int BME280_Init() {
	return BME280_Config(OSRS_1, OSRS_1, OSRS_1, MODE_FORCED, T_SB_0p5, IIR_OFF);
}

BME280_Data_t* BME280_Get_Data() {
	BME280_WakeUP();
	BME280_Measure(&data.temperature, &data.pressure, &data.humidity);
	return &data;
}


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
int BME280_Config (uint8_t osrs_t, uint8_t osrs_p, uint8_t osrs_h, uint8_t mode, uint8_t t_sb, uint8_t filter)
{
	// Read the Trimming parameters
	if (TrimRead() != 0)
	{
		return 1;
	}

	uint8_t datatowrite = 0;
	uint8_t datacheck = 0;

	// Reset the device
	datatowrite = 0xB6;  // reset sequence
	if (HAL_I2C_Mem_Write(&BME280_I2C, BME280_ADDRESS, RESET_REG, 1, &datatowrite, 1, 1000) != HAL_OK)
	{
		return 2;  //error NUM for debug
	}

	HAL_Delay (100);


	// write the humidity oversampling to 0xF2
	datatowrite = osrs_h;
	if (HAL_I2C_Mem_Write(&BME280_I2C, BME280_ADDRESS, CTRL_HUM_REG, 1, &datatowrite, 1, 1000) != HAL_OK)
	{
		return 3;  //error NUM for debug
	}
	HAL_Delay (100);
	HAL_I2C_Mem_Read(&BME280_I2C, BME280_ADDRESS, CTRL_HUM_REG, 1, &datacheck, 1, 1000);
	if (datacheck != datatowrite)
	{
		return 4;  //error NUM for debug
	}


	// write the standby time and IIR filter coeff to 0xF5
	datatowrite = (t_sb <<5) |(filter << 2);
	if (HAL_I2C_Mem_Write(&BME280_I2C, BME280_ADDRESS, CONFIG_REG, 1, &datatowrite, 1, 1000) != HAL_OK)
	{
		return 5;  //error NUM for debug
	}
	HAL_Delay (100);
	HAL_I2C_Mem_Read(&BME280_I2C, BME280_ADDRESS, CONFIG_REG, 1, &datacheck, 1, 1000);
	if (datacheck != datatowrite)
	{
		return 6;  //error NUM for debug
	}


	// write the pressure and temp oversampling along with mode to 0xF4
	datatowrite = (osrs_t <<5) |(osrs_p << 2) | mode;
	if (HAL_I2C_Mem_Write(&BME280_I2C, BME280_ADDRESS, CTRL_MEAS_REG, 1, &datatowrite, 1, 1000) != HAL_OK)
	{
		return 7;  //error NUM for debug
	}
	HAL_Delay (100);
	HAL_I2C_Mem_Read(&BME280_I2C, BME280_ADDRESS, CTRL_MEAS_REG, 1, &datacheck, 1, 1000);
	if ((datacheck >> 2) != (datatowrite >> 2)) //ignore mode setting, forced mode goes instantly to sleep mode throwing the check off
	{
		return 8;  //error NUM for debug
	}

	return 0;  //Success
}

/**
 * @brief  Wake the sensor from sleep for a forced measurement
 */
void BME280_WakeUP(void)
{
	uint8_t datatowrite = 0;

	// first read the register
	HAL_I2C_Mem_Read(&BME280_I2C, BME280_ADDRESS, CTRL_MEAS_REG, 1, &datatowrite, 1, 1000);

	// modify the data with the forced mode
	datatowrite = datatowrite | MODE_FORCED;

	// write the new data to the register
	HAL_I2C_Mem_Write(&BME280_I2C, BME280_ADDRESS, CTRL_MEAS_REG, 1, &datatowrite, 1, 1000);

	HAL_Delay (100);
}

/**
 * @brief  Perform a measurement and retrieve temperature, pressure, humidity
 * @param  temperature  Pointer to store temperature (°C)
 * @param  pressure     Pointer to store pressure (Pa)
 * @param  humidity     Pointer to store humidity (%RH)
 */
void BME280_Measure(float *temperature, float *pressure, float *humidity)
{
    if (BMEReadRaw() == 0)
    {
        if (tRaw == 0x800000) *temperature = 0; // temp disabled
        else
            *temperature = (BME280_compensate_T_int32(tRaw)) / 100.0f;

        if (pRaw == 0x800000) *pressure = 0; // pressure disabled
        else
        {
#if SUPPORT_64BIT
            *pressure = (BME280_compensate_P_int64(pRaw)) / 256.0f;
#elif SUPPORT_32BIT
            *pressure = (BME280_compensate_P_int32(pRaw));  // in Pa
#endif
        }

        if (hRaw == 0x8000) *humidity = 0; // humidity disabled
        else
            *humidity = (bme280_compensate_H_int32(hRaw)) / 1024.0f;
    }
    else
    {
        // if the device is detached
        *temperature = 0;
        *pressure    = 0;
        *humidity    = 0;
    }
}


/**
 * @brief  Read trimming parameters from sensor NVM
 * @retval 0 on success, non-zero on failure
 */
static int TrimRead(void)
{
	uint8_t trimdata[32];

	// Read NVM from 0x88 to 0xA1 (25 bytes)
	if (HAL_I2C_Mem_Read(&BME280_I2C, BME280_ADDRESS, 0x88, 1, trimdata, 25, HAL_MAX_DELAY) != HAL_OK)
	{
			return 1;  //error NUM for debug
	}

	// Read NVM from 0xE1 to 0xE7 (7 bytes)
	if (HAL_I2C_Mem_Read(&BME280_I2C, BME280_ADDRESS, 0xE1, 1, trimdata + 25, 7, HAL_MAX_DELAY) != HAL_OK)
	{
		return 2;  //error NUM for debug
	}

	// Temperature coefficients
	dig_T1 = (uint16_t)(trimdata[1] << 8 | trimdata[0]);
	dig_T2 = (int16_t)(trimdata[3] << 8 | trimdata[2]);
	dig_T3 = (int16_t)(trimdata[5] << 8 | trimdata[4]);

	// Pressure coefficients
	dig_P1 = (uint16_t)(trimdata[7] << 8 | trimdata[6]);
	dig_P2 = (int16_t)(trimdata[9] << 8 | trimdata[8]);
	dig_P3 = (int16_t)(trimdata[11] << 8 | trimdata[10]);
	dig_P4 = (int16_t)(trimdata[13] << 8 | trimdata[12]);
	dig_P5 = (int16_t)(trimdata[15] << 8 | trimdata[14]);
	dig_P6 = (int16_t)(trimdata[17] << 8 | trimdata[16]);
	dig_P7 = (int16_t)(trimdata[19] << 8 | trimdata[18]);
	dig_P8 = (int16_t)(trimdata[21] << 8 | trimdata[20]);
	dig_P9 = (int16_t)(trimdata[23] << 8 | trimdata[22]);

	// Humidity coefficients
	dig_H1 = trimdata[24];
	dig_H2 = (int16_t)(trimdata[26] << 8 | trimdata[25]);
	dig_H3 = trimdata[27];
	dig_H4 = (int16_t)((trimdata[28] << 4) | (trimdata[29] & 0x0F));
	dig_H5 = (int16_t)((trimdata[30] << 4) | (trimdata[29] >> 4));
	dig_H6 = (int8_t)trimdata[31];

	return 0; //Success
}

/**
 * @brief  Read raw ADC values from the sensor
 * @retval 0 on success, 1 if chip ID mismatch (device not found)
 */
static int BMEReadRaw(void)
{
	uint8_t RawData[8];

	// Check the chip ID before reading
	HAL_I2C_Mem_Read(&hi2c1, BME280_ADDRESS, ID_REG, 1, &chipID, 1, 1000);

	if (chipID == 0x60)
	{
		// Read the Registers 0xF7 to 0xFE
		HAL_I2C_Mem_Read(&BME280_I2C, BME280_ADDRESS, PRESS_MSB_REG, 1, RawData, 8, HAL_MAX_DELAY);

		/* Calculate the Raw data for the parameters
		 * Here the Pressure and Temperature are in 20 bit format and humidity in 16 bit format
		 */
		pRaw = (RawData[0]<<12)|(RawData[1]<<4)|(RawData[2]>>4);
		tRaw = (RawData[3]<<12)|(RawData[4]<<4)|(RawData[5]>>4);
		hRaw = (RawData[6]<<8)|(RawData[7]);

		return 0;
	}

	else return 1;  //error NUM for debug
}

/************* COMPENSATION CALCULATION AS PER DATASHEET (page 25) **************************/

/**
 * @brief  Compensate raw temperature using calibration data
 * @param  adc_T  Raw temperature ADC value
 * @return Temperature in 0.01 DegC (e.g. 5123 = 51.23 DegC)
 * @note   Sets global t_fine for pressure/humidity compensation
 */
static int32_t BME280_compensate_T_int32(int32_t adc_T)
{
	int32_t var1, var2, T;
	var1 = ((((adc_T>>3) - ((int32_t)dig_T1<<1))) * ((int32_t)dig_T2)) >> 11;
	var2 = (((((adc_T>>4) - ((int32_t)dig_T1)) * ((adc_T>>4) - ((int32_t)dig_T1)))>> 12) *((int32_t)dig_T3)) >> 14;
	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	return T;
}


#if SUPPORT_64BIT
/**
 * @brief  Compensate raw pressure using calibration data (64-bit)
 * @param  adc_P  Raw pressure ADC value
 * @return Pressure in Q24.8 format (e.g. 24674867 = 96386.2 Pa)
 */
static uint32_t BME280_compensate_P_int64(int32_t adc_P)
{
	int64_t var1, var2, p;
	var1 = ((int64_t)t_fine) - 128000;
	var2 = var1 * var1 * (int64_t)dig_P6;
	var2 = var2 + ((var1*(int64_t)dig_P5)<<17);
	var2 = var2 + (((int64_t)dig_P4)<<35);
	var1 = ((var1 * var1 * (int64_t)dig_P3)>>8) + ((var1 * (int64_t)dig_P2)<<12);
	var1 = (((((int64_t)1)<<47)+var1))*((int64_t)dig_P1)>>33;
	if (var1 == 0)
	{
		return 0; // avoid exception caused by division by zero
	}
	p = 1048576-adc_P;
	p = (((p<<31)-var2)*3125)/var1;
	var1 = (((int64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((int64_t)dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7)<<4);
	return (uint32_t)p;
}

#elif SUPPORT_32BIT
// Returns pressure in Pa as unsigned 32 bit integer. Output value of “96386” equals 96386 Pa = 963.86 hPa
static uint32_t BME280_compensate_P_int32(int32_t adc_P)
{
	int32_t var1, var2;
	uint32_t p;
	var1 = (((int32_t)t_fine)>>1) - (int32_t)64000;
	var2 = (((var1>>2) * (var1>>2)) >> 11 ) * ((int32_t)dig_P6);
	var2 = var2 + ((var1*((int32_t)dig_P5))<<1);
	var2 = (var2>>2)+(((int32_t)dig_P4)<<16);
	var1 = (((dig_P3 * (((var1>>2) * (var1>>2)) >> 13 )) >> 3) + ((((int32_t)dig_P2) *var1)>>1))>>18;
	var1 =((((32768+var1))*((int32_t)dig_P1))>>15);
	if (var1 == 0)
	{
		return 0; // avoid exception caused by division by zero
	}
	p = (((uint32_t)(((int32_t)1048576)-adc_P)-(var2>>12)))*3125;
	if (p < 0x80000000)
	{
		p = (p << 1) / ((uint32_t)var1);
	}
	else
	{
		p = (p / (uint32_t)var1) * 2;
	}
	var1 = (((int32_t)dig_P9) * ((int32_t)(((p>>3) * (p>>3))>>13)))>>12;
	var2 = (((int32_t)(p>>2)) * ((int32_t)dig_P8))>>13;
	p = (uint32_t)((int32_t)p + ((var1 + var2 + dig_P7) >> 4));
	return p;
}
#endif

/**
 * @brief  Compensate raw humidity using calibration data
 * @param  adc_H  Raw humidity ADC value
 * @return Humidity in Q22.10 format (e.g. 47445 = 46.333 %%RH)
 */
static uint32_t bme280_compensate_H_int32(int32_t adc_H)
{
	int32_t v_x1_u32r;
	v_x1_u32r = (t_fine - ((int32_t)76800));
	v_x1_u32r = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) *\
			v_x1_u32r)) + ((int32_t)16384)) >> 15) * (((((((v_x1_u32r *\
					((int32_t)dig_H6)) >> 10) * (((v_x1_u32r * ((int32_t)dig_H3)) >> 11) +\
							((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)dig_H2) +\
					8192) >> 14));
	v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *\
			((int32_t)dig_H1)) >> 4));
	v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
	v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
	return (uint32_t)(v_x1_u32r>>12);
}
/*********************************************************************************************************/
