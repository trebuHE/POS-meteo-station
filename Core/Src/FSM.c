#include "FSM.h"



extern ADC_HandleTypeDef hadc;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

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

uint8_t enable_gps = 0;

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
			break;
		case PROCESS_DATA:
			radio_data.id = 0x01;
			radio_data.utc = (uint32_t)170623.125f;  //GPS.utc_time;
			radio_data.date = GPS.day * 100000000 + GPS.month * 1000000 + GPS.year; // ddmmyyyy
			radio_data.temp = (uint16_t)(BME_data_p->temperature * 10.0f);
			radio_data.humidity = (uint16_t)(BME_data_p->humidity * 10.0f);
			radio_data.pressure = (uint16_t)(BME_data_p->pressure * 10.0f);
			radio_data.pm25 = PMS_data_p->frame.pm_2_5_atmos;
			radio_data.pm10 = PMS_data_p->frame.pm_1_0_atmos;
			radio_data.uv_index = (uint8_t)(*UV_index_p);
			state = TX;
			break;
		case TX:
			CC1101_Transmit((uint8_t*)(&radio_data), sizeof(radio_data));
			HAL_Delay(200);
			state = WAKEUP;
			break;
		case RX:
			break;
		default:
			break;
		}
	}
}

static uint8_t Init(UV_Config_t* uv_config_p) {
	BME280_Init();
	UV_Init(&UV_config);
	PMS_Init(&huart1);
	CC1101_Init();
	if (enable_gps) {
		GPS_Init(&huart2);
	}
	return 0;
}

static uint8_t modules_sleep() {
	HAL_GPIO_WritePin(PMS_SLEEP_GPIO_Port, PMS_SLEEP_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPS_WAKEUP_GPIO_Port, GPS_WAKEUP_Pin, GPIO_PIN_RESET);

	CC1101_CommandStrobe(CC1101_SIDLE);
	HAL_Delay(10);
	CC1101_CommandStrobe(CC1101_SPWD);
	HAL_Delay(10);

	return 0;
}

static uint8_t modules_wakeup() {
	HAL_GPIO_WritePin(PMS_SLEEP_GPIO_Port, PMS_SLEEP_Pin, GPIO_PIN_SET);

	if (enable_gps) {
		HAL_GPIO_WritePin(GPS_WAKEUP_GPIO_Port, GPS_WAKEUP_Pin, GPIO_PIN_SET);
	}

	CC1101_CommandStrobe(CC1101_SIDLE);
	HAL_Delay(10);

	return 0;
}
