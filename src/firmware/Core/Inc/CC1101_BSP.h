/**
 * @file    CC1101_BSP.h
 * @brief   CC1101 sub-1GHz RF transceiver driver (433 MHz)
 * @author  POS Meteo Station Team
 * @date    2025
 */

#ifndef CC1101_H
#define CC1101_H

#include "main.h"

/** @name SPI and GPIO Configuration */
/**@{*/
extern SPI_HandleTypeDef hspi1;
#define CC1101_SPI_HANDLE   hspi1

#define CC1101_CSN_PORT     GPIOA
#define CC1101_CSN_PIN      GPIO_PIN_4
#define CC1101_GDO0_PORT    GPIOC
#define CC1101_GDO0_PIN     GPIO_PIN_4
// GDO2 is defined but not strictly used in this simple implementation
#define CC1101_GDO2_PORT    GPIOC
#define CC1101_GDO2_PIN     GPIO_PIN_5
/**@}*/

/** @name SPI Transfer Macros */
/**@{*/
#define CC1101_WRITE_BYTE   0x00 /**< Write access bit */
#define CC1101_READ_BYTE    0x80 /**< Read access bit */
#define CC1101_BURST_BIT    0x40 /**< Burst access bit */
/**@}*/

/** @name Configuration Registers */
/**@{*/
#define CC1101_IOCFG2       0x00 /**< GDO2 output pin config */
#define CC1101_IOCFG0       0x02 /**< GDO0 output pin config */
#define CC1101_FIFOTHR      0x03 /**< FIFO threshold */
#define CC1101_PKTCTRL1     0x07 /**< Packet control 1 */
#define CC1101_PKTCTRL0     0x08 /**< Packet control 0 */
#define CC1101_ADDR			0x09 /**< Device address */
#define CC1101_FSCTRL1      0x0B /**< Frequency synthesizer control 1 */
#define CC1101_FREQ2        0x0D /**< Frequency word MSB */
#define CC1101_FREQ1        0x0E /**< Frequency word middle */
#define CC1101_FREQ0        0x0F /**< Frequency word LSB */
#define CC1101_MDMCFG4      0x10 /**< Modem config 4 */
#define CC1101_MDMCFG3      0x11 /**< Modem config 3 */
#define CC1101_MDMCFG2      0x12 /**< Modem config 2 */
#define CC1101_DEVIATN      0x15 /**< Deviation */
#define CC1101_MCSM0        0x18 /**< Main radio control state machine 0 */
#define CC1101_MCSM1		0x17 /**< Main radio control state machine 1 */
#define CC1101_FOCCFG       0x19 /**< Frequency offset compensation config */
#define CC1101_WORCTRL      0x20 /**< Wake-on-radio control */
#define CC1101_FSCAL3       0x23 /**< Frequency synthesizer calibration 3 */
#define CC1101_FSCAL2       0x24 /**< Frequency synthesizer calibration 2 */
#define CC1101_FSCAL1       0x25 /**< Frequency synthesizer calibration 1 */
#define CC1101_FSCAL0       0x26 /**< Frequency synthesizer calibration 0 */
/**@}*/

/** @name Command Strobes */
/**@{*/
#define CC1101_SRES         0x30 /**< Reset chip */
#define CC1101_SFSTXON      0x31 /**< Enable and calibrate frequency synth */
#define CC1101_SXOFF        0x32 /**< Turn off crystal oscillator */
#define CC1101_SCAL         0x33 /**< Calibrate frequency synth */
#define CC1101_SRX          0x34 /**< Enable RX mode */
#define CC1101_STX          0x35 /**< Enable TX mode */
#define CC1101_SIDLE        0x36 /**< Exit RX/TX, enter IDLE */
#define CC1101_SFRX         0x3A /**< Flush RX FIFO */
#define CC1101_SFTX         0x3B /**< Flush TX FIFO */
#define CC1101_SNOP         0x3D /**< No operation */
#define CC1101_SPWD			0x39 /**< Sleep mode */
/**@}*/

/** @name Status and FIFO Registers */
/**@{*/
#define CC1101_PARTNUM      0x30 /**< Part number status */
#define CC1101_VERSION      0x31 /**< Chip version status */
#define CC1101_RSSI         0x34 /**< RSSI value */
#define CC1101_RXBYTES      0x3B /**< RX FIFO byte count */
#define CC1101_TXBYTES      0x3C /**< TX FIFO byte count */
#define CC1101_FIFO         0x3F /**< FIFO access */
#define CC1101_PATABLE      0x3E /**< PA power table */
/**@}*/

/** @name Expected Chip Values */
/**@{*/
#define CC1101_PARTNUM_VAL  0x00 /**< Expected part number */
#define CC1101_VERSION_VAL  0x14 /**< Expected version (rev B) */
/**@}*/


/** @brief Radio packet payload from a measurement node */
typedef struct {
	uint8_t address;    /**< Node address */
	uint32_t utc;       /**< UTC timestamp */
	uint32_t date;      /**< Date in packed format (DDMMYYYY) */
	uint8_t gps_valid;  /**< GPS fix validity flag */
	int16_t temp;       /**< Temperature (x10, e.g. 235 = 23.5 C) */
	uint16_t humidity;  /**< Humidity (x10, e.g. 455 = 45.5 %) */
	uint16_t pressure;  /**< Pressure in hPa */
	uint16_t pm25;      /**< PM2.5 concentration */
	uint16_t pm10;      /**< PM10 concentration */
	uint8_t uv_index;   /**< UV index */
}Radio_Data_t;

/** @brief Radio configuration parameters */
typedef struct {
	uint8_t address;    /**< Node address */
	uint8_t interval;   /**< Measurement interval in minutes */
}Radio_Config_t;



/**
 * @brief  Initialise the CC1101 (reset + verify + configure)
 * @retval 1 on success, 0 on failure
 */
uint8_t CC1101_Init(void);

/**
 * @brief  Apply 433 MHz radio configuration settings
 */
void CC1101_Config(void);

/**
 * @brief  Transmit a packet
 * @param  data    Pointer to data buffer
 * @param  length  Number of bytes to transmit
 */
void CC1101_Transmit(uint8_t *data, uint8_t length);

/**
 * @brief  Receive a packet (non-blocking, checks FIFO)
 * @param  data  Pointer to buffer for received payload
 * @return Length of received packet, 0 if none or CRC fail
 */
uint8_t CC1101_Receive(uint8_t *data);

/**
 * @brief  Enter RX listening mode
 */
void CC1101_SetRx(void);

/**
 * @brief  Read the current radio state (MARCSTATE)
 * @return State value (masked to lower 5 bits)
 */
uint8_t CC1101_GetState(void);

/**
 * @brief  Send a command strobe to the radio
 * @param  cmd  Command strobe byte (CC1101_SRES, CC1101_SRX, etc.)
 */
void CC1101_CommandStrobe(uint8_t cmd);

/**
 * @brief  Get number of bytes in TX FIFO
 * @return TX FIFO byte count
 */
uint8_t CC1101_GetTXBytes(void);

/**
 * @brief  Print radio state info over UART (debug)
 */
void debug_radio_state();

#endif // CC1101_H
