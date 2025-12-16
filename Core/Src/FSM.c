#include "FSM.h"

extern ADC_HandleTypeDef hadc;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart4;
extern RTC_HandleTypeDef hrtc;

extern GPS_t GPS;

static const uint16_t GPS_COUNTER_MAX = 1;

FSM_State_t state = WAKEUP;

UV_Config_t UV_config = {
		.adc_max_value = 4095,
		.adc_max_voltage = 3.3f,
		.adc_p = &hadc,
		.index_thresholds_mV = {50, 227, 318, 408, 503, 606, 696, 795, 881, 976, 1069}
};

BME280_Data_t* BME_data_p;
UV_Index_t* UV_index_p;
PMS_Data_t* PMS_data_p;
Radio_Data_t radio_data = {0};
uint8_t measure_period_min = 15;
uint8_t enable_gps = 0;
uint8_t gps_counter = GPS_COUNTER_MAX;
uint8_t sleep_time_min = 1;

RTC_DateTypeDef RTC_date;
RTC_TimeTypeDef RTC_time;

void FSM_Run() {

	while(1){

		switch (state) {
		case IDLE:
			__HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);
			__HAL_RTC_WAKEUPTIMER_EXTI_CLEAR_FLAG();
			HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, (sleep_time_min * 60), RTC_WAKEUPCLOCK_CK_SPRE_16BITS);

			HAL_GPIO_WritePin(TEST_LED_GPIO_Port, TEST_LED_Pin, GPIO_PIN_RESET);
			__HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);
			HAL_SuspendTick();
			HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
			state = WAKEUP;
			break;
		case WAKEUP:
			SystemClock_Config();
			HAL_ResumeTick();
			HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
			__HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);
			__HAL_RTC_WAKEUPTIMER_EXTI_CLEAR_FLAG();
			if (gps_counter-- == 0) {
				enable_gps = 1;
			}
			modules_wakeup();
			HAL_Delay(100);
			Init(&UV_config);
			HAL_GPIO_WritePin(TEST_LED_GPIO_Port, TEST_LED_Pin, GPIO_PIN_SET);
			state = MEASURE;
			break;
		case MEASURE:
			for (uint8_t i = 0; i < 30; ++i) {
				HAL_GPIO_WritePin(TEST_LED_GPIO_Port, TEST_LED_Pin, GPIO_PIN_RESET);
				HAL_Delay(500);
				HAL_GPIO_WritePin(TEST_LED_GPIO_Port, TEST_LED_Pin, GPIO_PIN_SET);
				HAL_Delay(500);
			}

			BME_data_p = BME280_Get_Data();
			UV_index_p = UV_Get_Index();

			PMS_data_p = PMS_Get_Data();
			if (!PMS_Is_Data_Valid(PMS_data_p)) {
				HAL_GPIO_WritePin(TEST_LED_GPIO_Port, TEST_LED_Pin, GPIO_PIN_RESET);
			}

			modules_sleep();

			if (enable_gps) {
				state = SYNC_TIME;
			} else {
				state = PROCESS_DATA;
			}
			break;
		case SYNC_TIME:

			for (uint8_t i = 0; i < 60; ++i) {
				HAL_GPIO_WritePin(TEST_LED_GPIO_Port, TEST_LED_Pin, GPIO_PIN_RESET);
				HAL_Delay(250);
				HAL_GPIO_WritePin(TEST_LED_GPIO_Port, TEST_LED_Pin, GPIO_PIN_SET);
				HAL_Delay(250);
			}

			RTC_set_time(GPS_get_hours(), GPS_get_minutes(), GPS_get_seconds());
			RTC_set_date(GPS_get_day(), GPS_get_month(), (GPS_get_year() - 2000));
			enable_gps = 0;
			gps_counter = GPS_COUNTER_MAX;
			HAL_GPIO_WritePin(GPS_WAKEUP_GPIO_Port, GPS_WAKEUP_Pin, GPIO_PIN_RESET);
			state = PROCESS_DATA;

			break;
		case PROCESS_DATA:
			radio_data.address = 0x05;
			RTC_get_time_date(&RTC_date, &RTC_time);
			radio_data.utc = RTC_time.Hours * 10000 + RTC_time.Minutes * 100 + RTC_time.Seconds; //hhmmss
			radio_data.date = RTC_date.Date* 1000000 + RTC_date.Month * 10000 + RTC_date.Year + 2000; // ddmmyyyy
			radio_data.gps_valid = 1;
			radio_data.temp = (int16_t)(BME_data_p->temperature * 10.0f);
			radio_data.humidity = (uint16_t)(BME_data_p->humidity * 10.0f);
			radio_data.pressure = (uint16_t)(BME_data_p->pressure/100.0f);
			radio_data.pm25 = PMS_data_p->frame.pm_2_5_atmos;
			radio_data.pm10 = PMS_data_p->frame.pm_1_0_atmos;
			radio_data.uv_index = (uint8_t)(*UV_index_p);
			state = TX;
			break;
		case TX:
			CC1101_Transmit((uint8_t*)(&radio_data), sizeof(radio_data));
			HAL_Delay(200);
			Debug_Radio_Data(&huart4, &radio_data);
			state = IDLE;
			break;
		case RX:
			break;
		default:
			break;
		}
	}
}

uint8_t Init(UV_Config_t* uv_config_p) {
	BME280_Init();
	UV_Init(&UV_config);
	PMS_Init(&huart1);
	CC1101_Init();
	if (enable_gps) {
		GPS_Init(&huart2);
	}
	return 0;
}

uint8_t modules_sleep() {
	if (enable_gps != 1)
		HAL_GPIO_WritePin(GPS_WAKEUP_GPIO_Port, GPS_WAKEUP_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(PMS_SLEEP_GPIO_Port, PMS_SLEEP_Pin, GPIO_PIN_RESET);

	CC1101_CommandStrobe(CC1101_SIDLE);
	HAL_Delay(10);
	CC1101_CommandStrobe(CC1101_SPWD);
	HAL_Delay(10);

	return 0;
}

uint8_t modules_wakeup() {
	HAL_GPIO_WritePin(PMS_SLEEP_GPIO_Port, PMS_SLEEP_Pin, GPIO_PIN_SET);

	if (enable_gps) {
		HAL_GPIO_WritePin(GPS_WAKEUP_GPIO_Port, GPS_WAKEUP_Pin, GPIO_PIN_SET);
	}

	CC1101_CommandStrobe(CC1101_SIDLE);
	HAL_Delay(10);

	return 0;
}

void RTC_set_time(uint8_t h, uint8_t m, uint8_t s) {
	RTC_TimeTypeDef sTime = {0};
	sTime.Hours = h;
	sTime.Minutes = m;
	sTime.Seconds = s;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
	{
		//Error_Handler();
	}
}

void RTC_set_date(uint8_t d,  uint8_t m, uint8_t y) {
	RTC_DateTypeDef sDate = {0};
	sDate.WeekDay = 1;
	sDate.Month = m;
	sDate.Date = d;
	sDate.Year = y;
	if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
	{
		//Error_Handler();
	}
}

void RTC_get_time_date(RTC_DateTypeDef* date_p, RTC_TimeTypeDef* time_p) {
	HAL_RTC_GetTime(&hrtc, time_p, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, date_p, RTC_FORMAT_BIN);
}

void Debug_Radio_Data(UART_HandleTypeDef *huart, Radio_Data_t *data) {
    char buff[256]; // Large buffer to hold the full report
    int len;

    // Format the data with labels
    // \r\n creates a new line in serial terminals
    len = snprintf(buff, sizeof(buff),
        "--- Sensor Report ---\r\n"
        "Address  : %u\r\n"
        "UTC      : %lu\r\n"
        "Date     : %lu\r\n"
		"GPS Valid: %d\r\n"
        "Temp     : %d\r\n"
        "Humidity : %u\r\n"
        "Pressure : %u\r\n"
        "PM2.5    : %u\r\n"
        "PM10     : %u\r\n"
        "UV Index : %u\r\n"
        "---------------------\r\n\n",
        data->address,
        data->utc,
        data->date,
		data->gps_valid,
        data->temp,
        data->humidity,
        data->pressure,
        data->pm25,
        data->pm10,
        data->uv_index
    );

    HAL_UART_Transmit(huart, (uint8_t*)buff, (uint16_t)len, 100);
}
