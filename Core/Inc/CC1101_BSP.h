#ifndef CC1101_H
#define CC1101_H

#include "main.h"

// --- SPI / GPIO Configuration ---
extern SPI_HandleTypeDef hspi1;
#define CC1101_SPI_HANDLE   hspi1

#define CC1101_CSN_PORT     GPIOA
#define CC1101_CSN_PIN      GPIO_PIN_4
#define CC1101_GDO0_PORT    GPIOC
#define CC1101_GDO0_PIN     GPIO_PIN_4
// GDO2 is defined but not strictly used in this simple implementation
#define CC1101_GDO2_PORT    GPIOC
#define CC1101_GDO2_PIN     GPIO_PIN_5

// --- Macros ---
#define CC1101_WRITE_BYTE   0x00
#define CC1101_READ_BYTE    0x80
#define CC1101_BURST_BIT    0x40

// --- Configuration Registers ---
#define CC1101_IOCFG2       0x00
#define CC1101_IOCFG0       0x02
#define CC1101_FIFOTHR      0x03
#define CC1101_PKTCTRL1     0x07
#define CC1101_PKTCTRL0     0x08
#define CC1101_ADDR			0x09
#define CC1101_FSCTRL1      0x0B
#define CC1101_FREQ2        0x0D
#define CC1101_FREQ1        0x0E
#define CC1101_FREQ0        0x0F
#define CC1101_MDMCFG4      0x10
#define CC1101_MDMCFG3      0x11
#define CC1101_MDMCFG2      0x12
#define CC1101_DEVIATN      0x15
#define CC1101_MCSM0        0x18
#define CC1101_MCSM1		0x17
#define CC1101_FOCCFG       0x19
#define CC1101_WORCTRL      0x20
#define CC1101_FSCAL3       0x23
#define CC1101_FSCAL2       0x24
#define CC1101_FSCAL1       0x25
#define CC1101_FSCAL0       0x26

// --- Command Strobes ---
#define CC1101_SRES         0x30
#define CC1101_SFSTXON      0x31
#define CC1101_SXOFF        0x32
#define CC1101_SCAL         0x33
#define CC1101_SRX          0x34 // Enable RX
#define CC1101_STX          0x35 // Enable TX
#define CC1101_SIDLE        0x36 // Exit RX/TX
#define CC1101_SFRX         0x3A // Flush RX FIFO
#define CC1101_SFTX         0x3B // Flush TX FIFO
#define CC1101_SNOP         0x3D
#define CC1101_SPWD			0x39 // Sleep Mode

// --- Status & FIFO ---
#define CC1101_PARTNUM      0x30
#define CC1101_VERSION      0x31
#define CC1101_RSSI         0x34
#define CC1101_RXBYTES      0x3B
#define CC1101_TXBYTES      0x3C
#define CC1101_FIFO         0x3F
#define CC1101_PATABLE      0x3E

// --- Expected Values ---
#define CC1101_PARTNUM_VAL  0x00
#define CC1101_VERSION_VAL  0x14


typedef struct {
	uint8_t id;
	uint32_t utc;
	uint32_t date;
	int16_t temp;
	uint16_t humidity;
	uint16_t pressure;
	uint16_t pm25;
	uint16_t pm10;
	uint8_t uv_index;
}Radio_Data_t;



// --- Function Prototypes ---
uint8_t CC1101_Init(void); // Reset + Config
void CC1101_Config(void);  // Apply 433MHz settings

// Simple TX/RX
void CC1101_Transmit(uint8_t *data, uint8_t length);
uint8_t CC1101_Receive(uint8_t *data); // Returns length, 0 if no packet
void CC1101_SetRx(void); // Enter RX mode
uint8_t CC1101_GetState(void);
void CC1101_CommandStrobe(uint8_t cmd);
uint8_t CC1101_GetTXBytes(void);
void debug_radio_state();

#endif // CC1101_H
