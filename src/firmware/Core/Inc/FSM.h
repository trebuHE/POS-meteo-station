/**
 * @file    FSM.h
 * @brief   Application finite state machine
 * @author  POS Meteo Station Team
 * @date    2025
 */

#ifndef INC_FSM_H_
#define INC_FSM_H_

#include "main.h"
#include "BME280_STM32.h"
#include "UV.h"
#include "gps.h"
#include "PMS.h"
#include "CC1101_BSP.h"
#include <stdio.h>

/** @brief  Main application FSM states */
typedef enum {
	IDLE,          /**< Sleep / low-power, waiting for RTC wakeup */
	WAKEUP,        /**< System wakeup, clock restore, module power-on */
	MEASURE,       /**< Perform sensor measurements (BME, UV, PMS) */
	SYNC_TIME,     /**< Synchronise RTC via GPS */
	PROCESS_DATA,  /**< Package sensor data into radio frame */
	TX,            /**< Transmit data via CC1101 radio */
	RX             /**< Listen for configuration reply via CC1101 */
} FSM_State_t;

/**
 * @brief  Main FSM loop (runs forever, never returns)
 */
void FSM_Run(void);

/**
 * @brief  Initialise all sensor modules
 * @param  uv_config_p  Pointer to UV configuration
 * @return 0 on success
 */
uint8_t Init(UV_Config_t* uv_config_p);

/**
 * @brief  Put all sensor modules into sleep/low-power mode
 * @return 0 on success
 */
uint8_t modules_sleep(void);

/**
 * @brief  Wake all sensor modules from sleep
 * @return 0 on success
 */
uint8_t modules_wakeup(void);

/**
 * @brief  Print radio packet data over UART for debugging
 * @param  huart  UART handle for output
 * @param  data   Pointer to radio data to print
 */
void Debug_Radio_Data(UART_HandleTypeDef *huart, Radio_Data_t *data);

/**
 * @brief  Set the RTC time
 * @param  h  Hours (0-23)
 * @param  m  Minutes (0-59)
 * @param  s  Seconds (0-59)
 */
void RTC_set_time(uint8_t h, uint8_t m, uint8_t s);

/**
 * @brief  Set the RTC date
 * @param  d  Day (1-31)
 * @param  m  Month (1-12)
 * @param  y  Year (0-99, offset from 2000)
 */
void RTC_set_date(uint8_t d, uint8_t m, uint8_t y);

/**
 * @brief  Get the current RTC time and date
 * @param  date_p  Pointer to store date
 * @param  time_p  Pointer to store time
 */
void RTC_get_time_date(RTC_DateTypeDef* date_p, RTC_TimeTypeDef* time_p);

#endif /* INC_FSM_H_ */
