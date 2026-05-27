/**
 * @file    gps.h
 * @brief   GPS NMEA parser driver (UART-based)
 * @author  Bulanov Konstantin
 * @date    Nov 15, 2019
 */

/** @brief  DMA buffer size for GPS UART reception (must be even) */
#define GPS_BUF_SIZE  128

/** @brief  Parsed GPS time and date data */
typedef struct{
    float utc_time;          /**< UTC time in hhmmss format */
    unsigned int day;        /**< Day of month (1-31) */
    unsigned int month;      /**< Month (1-12) */
    unsigned int year;       /**< Year */
    uint8_t data_valid;      /**< Data validity flag (1 = valid) */
} GPS_t;

/**
 * @brief  Initialise the GPS module (configure NMEA messages + start DMA)
 * @param  gps_uart  Pointer to UART handle connected to the GPS
 */
void GPS_Init(UART_HandleTypeDef* gps_uart);

/**
 * @brief  Validate an NMEA sentence using its checksum
 * @param  nmeastr  Pointer to the NMEA string
 * @return 1 if checksum is valid, 0 otherwise
 */
int GPS_validate(char *nmeastr);

/**
 * @brief  Parse a validated NMEA sentence ($GPZDA) for time and date
 * @param  GPSstrParse  Pointer to the NMEA string to parse
 */
void GPS_parse(char *GPSstrParse);

/**
 * @brief  Append NMEA checksum and CRLF to a command string
 * @param  str   Pointer to the command string (without checksum)
 * @param  size  Total buffer size
 */
void GPS_Append_NMEA_Checksum(char *str, size_t size);

/**
 * @brief  DMA half-transfer callback — copy first half of buffer
 */
void GPS_Half(void);

/**
 * @brief  DMA full-transfer callback — copy second half of buffer
 */
void GPS_Full(void);

/**
 * @brief  Process the copied DMA buffer and extract valid NMEA sentences
 */
void GPS_process_data(void);

/**
 * @brief  Invalidate the GPS data flag
 */
void GPS_invalidate(void);

/** @name GPS Time Getters */
/**@{*/
uint8_t GPS_get_hours(void);
uint8_t GPS_get_minutes(void);
uint8_t GPS_get_seconds(void);
/**@}*/

/** @name GPS Date Getters */
/**@{*/
uint8_t GPS_get_day(void);
uint8_t GPS_get_month(void);
uint8_t GPS_get_year(void);
/**@}*/

/**
 * @brief  Check if GPS data is currently valid
 * @return 1 if data is valid, 0 otherwise
 */
uint8_t GPS_is_data_valid(void);
