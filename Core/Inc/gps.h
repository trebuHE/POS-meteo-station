/*
 * gps.h
 *
 *  Created on: Nov 15, 2019
 *      Author: Bulanov Konstantin
 */

#define GPS_BUF_SIZE  128 // Must be an even number!

typedef struct{
    float utc_time;
    unsigned int day;
    unsigned int month;
    unsigned int year;
    uint8_t data_valid;
} GPS_t;


void GPS_Init(UART_HandleTypeDef* gps_uart);
int GPS_validate(char *nmeastr);
void GPS_parse(char *GPSstrParse);
void GPS_Append_NMEA_Checksum(char *str, size_t size);
void GPS_Half(void);
void GPS_Full(void);
void GPS_process_data(void);
void GPS_invalidate();

uint8_t GPS_get_hours();
uint8_t GPS_get_minutes();
uint8_t GPS_get_seconds();
uint8_t GPS_get_day();
uint8_t GPS_get_month();
uint8_t GPS_get_year();
uint8_t GPS_is_data_valid();
