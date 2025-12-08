#include "FSM.h"



extern ADC_HandleTypeDef hadc;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart4;

extern GPS_t GPS;

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

uint8_t enable_gps = 1;

void FSM_Run() {

	while(1){

		switch (state) {
		case IDLE:
			break;
		case WAKEUP:
			modules_wakeup();
			HAL_Delay(100);
			Init(&UV_config);

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

			for (uint8_t i = 0; i < 6; ++i) {
				HAL_GPIO_WritePin(TEST_LED_GPIO_Port, TEST_LED_Pin, GPIO_PIN_RESET);
				HAL_Delay(250);
				HAL_GPIO_WritePin(TEST_LED_GPIO_Port, TEST_LED_Pin, GPIO_PIN_SET);
				HAL_Delay(250);
			}

			state = PROCESS_DATA;

			break;
		case PROCESS_DATA:
			radio_data.id = 0x01;
			radio_data.utc = (uint32_t)GPS.utc_time;
			radio_data.date = GPS.day * 1000000 + GPS.month * 10000 + GPS.year; // ddmmyyyy
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
			state = WAKEUP;
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
	HAL_GPIO_WritePin(PMS_SLEEP_GPIO_Port, PMS_SLEEP_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPS_WAKEUP_GPIO_Port, GPS_WAKEUP_Pin, GPIO_PIN_RESET);

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

void Debug_Radio_Data(UART_HandleTypeDef *huart, Radio_Data_t *data) {
    char buff[256]; // Large buffer to hold the full report
    int len;

    // Format the data with labels
    // \r\n creates a new line in serial terminals
    len = snprintf(buff, sizeof(buff),
        "--- Sensor Report ---\r\n"
        "ID       : %u\r\n"
        "UTC      : %lu\r\n"
        "Date     : %lu\r\n"
        "Temp     : %d\r\n"
        "Humidity : %u\r\n"
        "Pressure : %u\r\n"
        "PM2.5    : %u\r\n"
        "PM10     : %u\r\n"
        "UV Index : %u\r\n"
        "---------------------\r\n\n",
        data->id,
        data->utc,
        data->date,
        data->temp,
        data->humidity,
        data->pressure,
        data->pm25,
        data->pm10,
        data->uv_index
    );

    HAL_UART_Transmit(huart, (uint8_t*)buff, (uint16_t)len, 100);
}
