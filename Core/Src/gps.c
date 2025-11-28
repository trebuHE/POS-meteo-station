/*
 * gps.c
 *
 *  Created on: Nov 15, 2019
 *      Author: Bulanov Konstantin
 *
 *  Contact information
 *  -------------------
 *
 * e-mail   :  leech001@gmail.com
 */

/*
 * |---------------------------------------------------------------------------------
 * | Copyright (C) Bulanov Konstantin,2019
 * |
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * |
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |---------------------------------------------------------------------------------
 */


#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <usart.h>
#include "gps.h"


UART_HandleTypeDef* p_gps_huart;

uint8_t dma_rx_buffer[GPS_BUF_SIZE] = {0};
uint8_t parsing_buffer[(GPS_BUF_SIZE/2)] = {0};

GPS_t GPS;

void GPS_Init(UART_HandleTypeDef* gps_uart) {
	p_gps_huart = gps_uart;
	char gps[80] = "$PCAS04,7";
	GPS_Append_NMEA_Checksum(gps, sizeof(gps));
	HAL_UART_Transmit(gps_uart, (uint8_t*)gps, strlen(gps), HAL_MAX_DELAY);
	HAL_Delay(200);
	char msg[80] = "$PCAS03,0,0,0,0,0,0,1,0,0,0,,,0,0";
	GPS_Append_NMEA_Checksum(msg, sizeof(msg));
	HAL_UART_Transmit(gps_uart, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
	HAL_Delay(50);
	HAL_UART_Receive_DMA(p_gps_huart, dma_rx_buffer, GPS_BUF_SIZE);
}

void GPS_Append_NMEA_Checksum(char *str, size_t size) {
    if (!str || size < 6) return;

    size_t len = strlen(str);
    if (len == 0 || len + 5 >= size) return; // Not enough space

    // Compute checksum: XOR of all chars between '$' and '*'
    uint8_t checksum = 0;
    const char *ptr = str;

    if (*ptr == '$') ptr++; // skip initial $

    while (*ptr && *ptr != '*')
        checksum ^= (uint8_t)*ptr++;

    int written = snprintf(str + len, size - len, "*%02X\n\r", checksum);

    if (written < 0 || (size_t)written >= size - len)
        str[size - 1] = '\0';
}



void GPS_Half() {
	memcpy(parsing_buffer, dma_rx_buffer, GPS_BUF_SIZE/2 - 1);

	GPS_process_data();
}

void GPS_Full() {
	memcpy(parsing_buffer, dma_rx_buffer + GPS_BUF_SIZE/2, GPS_BUF_SIZE/2 - 1);

	GPS_process_data();
}

void GPS_process_data() {
	uint8_t line[100] = {0};
	uint8_t line_index = 0;
	uint8_t in_line= 0;
	for(uint16_t i = 0; i<GPS_BUF_SIZE/2 - 1; i++) {
		if(parsing_buffer[i] == '$'){
			in_line = 1;
		} else if(parsing_buffer[i] == '\n') {
			in_line = 0;
			if(GPS_validate((char*)line)) {
				GPS_parse((char*)line);
				memset(line, 0, 100);
				line_index = 0;
			}
		}

		if(in_line) {
			line[line_index++] = parsing_buffer[i];
		}
	}
}

int GPS_validate(char *nmeastr){
    char check[3];
    char checkcalcstr[3];
    int i;
    int calculated_check;

    i=0;
    calculated_check=0;

    // check to ensure that the string starts with a $
    if(nmeastr[i] == '$')
        i++;
    else
        return 0;

    //No NULL reached, 75 char largest possible NMEA message, no '*' reached
    while((nmeastr[i] != 0) && (nmeastr[i] != '*') && (i < 75)){
        calculated_check ^= nmeastr[i];// calculate the checksum
        i++;
    }

    if(i >= 75){
        return 0;// the string was too long so return an error
    }

    if (nmeastr[i] == '*'){
        check[0] = nmeastr[i+1];    //put hex chars in check string
        check[1] = nmeastr[i+2];
        check[2] = 0;
    }
    else
        return 0;// no checksum separator found there for invalid

    sprintf(checkcalcstr,"%02X",calculated_check);
    return((checkcalcstr[0] == check[0])
        && (checkcalcstr[1] == check[1])) ? 1 : 0 ;
}

void GPS_parse(char *GPSstrParse){
    if (!strncmp(GPSstrParse, "$GNZDA", 6)){
    	sscanf(GPSstrParse, "$GNZDA,%f,%u,%u,%u", &GPS.utc_time, &GPS.day, &GPS.month, &GPS.year);
	}
}
