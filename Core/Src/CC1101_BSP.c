#include "CC1101_BSP.h"
#include <stdio.h>

extern UART_HandleTypeDef huart4;

// --- Low Level SPI Wrappers ---

void CC1101_Select(void) {
    HAL_GPIO_WritePin(CC1101_CSN_PORT, CC1101_CSN_PIN, GPIO_PIN_RESET);
}

void CC1101_Deselect(void) {
    HAL_GPIO_WritePin(CC1101_CSN_PORT, CC1101_CSN_PIN, GPIO_PIN_SET);
}

void CC1101_CommandStrobe(uint8_t cmd) {
    CC1101_Select();
    HAL_SPI_Transmit(&CC1101_SPI_HANDLE, &cmd, 1, 100);
    CC1101_Deselect();
}

void CC1101_WriteReg(uint8_t reg, uint8_t value) {
    uint8_t data[2] = {reg | CC1101_WRITE_BYTE, value};
    CC1101_Select();
    HAL_SPI_Transmit(&CC1101_SPI_HANDLE, data, 2, 100);
    CC1101_Deselect();
}

void CC1101_WriteBurst(uint8_t reg, uint8_t *buf, uint8_t len) {
    uint8_t addr = reg | CC1101_WRITE_BYTE | CC1101_BURST_BIT;
    CC1101_Select();
    HAL_SPI_Transmit(&CC1101_SPI_HANDLE, &addr, 1, 100);
    HAL_SPI_Transmit(&CC1101_SPI_HANDLE, buf, len, 100);
    CC1101_Deselect();
}

uint8_t CC1101_ReadReg(uint8_t reg) {
    uint8_t txByte = reg | CC1101_READ_BYTE;
    uint8_t rxByte = 0;
    CC1101_Select();
    HAL_SPI_Transmit(&CC1101_SPI_HANDLE, &txByte, 1, 100);
    HAL_SPI_Receive(&CC1101_SPI_HANDLE, &rxByte, 1, 100);
    CC1101_Deselect();
    return rxByte;
}

void CC1101_ReadBurst(uint8_t reg, uint8_t *buf, uint8_t len) {
    uint8_t addr = reg | CC1101_READ_BYTE | CC1101_BURST_BIT;
    CC1101_Select();
    HAL_SPI_Transmit(&CC1101_SPI_HANDLE, &addr, 1, 100);
    HAL_SPI_Receive(&CC1101_SPI_HANDLE, buf, len, 100);
    CC1101_Deselect();
}

uint8_t CC1101_ReadStatus(uint8_t reg) {
    uint8_t txByte = reg | CC1101_READ_BYTE | CC1101_BURST_BIT;
    uint8_t rxByte = 0;
    CC1101_Select();
    HAL_SPI_Transmit(&CC1101_SPI_HANDLE, &txByte, 1, 100);
    HAL_SPI_Receive(&CC1101_SPI_HANDLE, &rxByte, 1, 100);
    CC1101_Deselect();
    return rxByte;
}

// --- Configuration ---

void CC1101_Config(void) {
    // 1. Reset Chip
    CC1101_CommandStrobe(CC1101_SRES);

    // 2. IO Config
    // GDO0: Asserts on Sync Word, De-asserts on End of Packet
    CC1101_WriteReg(CC1101_IOCFG0, 0x06);

    // 3. FIFO & Packet Control
    CC1101_WriteReg(CC1101_FIFOTHR, 0x47);  // Threshold: 32 bytes
    CC1101_WriteReg(CC1101_ADDR, 0x10);     // Device Address: 0x10
    CC1101_WriteReg(CC1101_PKTCTRL0, 0x05); // Variable Length, CRC Enabled
    // Append Status (RSSI/LQI) + Address Check (0x00 is Broadcast)
    CC1101_WriteReg(CC1101_PKTCTRL1, 0x06);

    // 4. Frequency (433.92 MHz)
    CC1101_WriteReg(CC1101_FREQ2, 0x10);
    CC1101_WriteReg(CC1101_FREQ1, 0xB1);
    CC1101_WriteReg(CC1101_FREQ0, 0x3B);

    // 5. Modem Config (1.2 kBaud, GFSK, ~58kHz RX BW)
    CC1101_WriteReg(CC1101_MDMCFG4, 0xF5);
    CC1101_WriteReg(CC1101_MDMCFG3, 0x83);
    CC1101_WriteReg(CC1101_MDMCFG2, 0x11); // 15/16 Sync Bits detected
    CC1101_WriteReg(CC1101_DEVIATN, 0x15);

    // 6. State Machine (MCSM)
    // Calibrate on IDLE->RX/TX transition
    CC1101_WriteReg(CC1101_MCSM0, 0x18);
    // Stay in RX after receiving a packet (Critical for RX mode)
    CC1101_WriteReg(CC1101_MCSM1, 0x0F);

    // 7. Power Settings (0 dBm)
    CC1101_WriteReg(CC1101_FOCCFG, 0x16);
    CC1101_WriteReg(CC1101_WORCTRL, 0xFB);

    uint8_t paTable[] = {0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    CC1101_WriteBurst(CC1101_PATABLE, paTable, 8);
}

uint8_t CC1101_Init(void) {
    // Hard Reset Pulse
    CC1101_Deselect();
    HAL_Delay(1);
    CC1101_Select();
    HAL_Delay(1);
    CC1101_Deselect();
    HAL_Delay(1);

    CC1101_CommandStrobe(CC1101_SRES);
    HAL_Delay(10);

    // Verify Chip Connectivity
    if (CC1101_ReadStatus(CC1101_PARTNUM) != CC1101_PARTNUM_VAL) return 0;
    if (CC1101_ReadStatus(CC1101_VERSION) != CC1101_VERSION_VAL) return 0;

    CC1101_Config();
    return 1;
}

// --- Operation ---

void CC1101_SetRx(void) {
    CC1101_CommandStrobe(CC1101_SRX);
}

void CC1101_Transmit(uint8_t *data, uint8_t length) {
    CC1101_CommandStrobe(CC1101_SIDLE);
    CC1101_CommandStrobe(CC1101_SFTX); // Flush TX buffer

    // Format: [Length] [Data...]
    CC1101_WriteReg(CC1101_FIFO, length);
    CC1101_WriteBurst(CC1101_FIFO, data, length);

    CC1101_CommandStrobe(CC1101_STX);

    // Wait for Sync Word (GDO0 High) then End of Packet (GDO0 Low)
    while (HAL_GPIO_ReadPin(CC1101_GDO0_PORT, CC1101_GDO0_PIN) == GPIO_PIN_RESET);
    while (HAL_GPIO_ReadPin(CC1101_GDO0_PORT, CC1101_GDO0_PIN) == GPIO_PIN_SET);

    CC1101_CommandStrobe(CC1101_SIDLE);
}

uint8_t CC1101_Receive(uint8_t *data) {
    uint8_t rxBytes = CC1101_ReadStatus(CC1101_RXBYTES) & 0x7F;

    // Check if we have at least the Length byte
    if (rxBytes > 1) {
        uint8_t len = CC1101_ReadReg(CC1101_FIFO);

        // Sanity Check: If length is unreasonably large, it's noise
        if (len > 60) {
            CC1101_CommandStrobe(CC1101_SIDLE);
            CC1101_CommandStrobe(CC1101_SFRX);
            CC1101_SetRx();
            return 0;
        }

        // Wait for the full packet to arrive (Payload + 2 Status Bytes)
        uint8_t bytesNeeded = len + 2;
        uint32_t timeout = HAL_GetTick() + 500;

        while (1) {
            rxBytes = CC1101_ReadStatus(CC1101_RXBYTES) & 0x7F;
            if (rxBytes >= bytesNeeded) break;

            if (HAL_GetTick() > timeout) {
                CC1101_CommandStrobe(CC1101_SIDLE);
                CC1101_CommandStrobe(CC1101_SFRX);
                CC1101_SetRx();
                return 0;
            }
            HAL_Delay(2);
        }

        // Read Payload
        CC1101_ReadBurst(CC1101_FIFO, data, len);

        // Read & Check Status Bytes (RSSI + CRC_OK)
        uint8_t status[2];
        CC1101_ReadBurst(CC1101_FIFO, status, 2);

        // CRC Check (Bit 7 of 2nd status byte)
        if (!(status[1] & 0x80)) {
            CC1101_SetRx(); // CRC Failed, ignore packet
            return 0;
        }

        CC1101_SetRx(); // Return to listening
        return len;
    }
    return 0;
}

// --- Debugging ---

uint8_t CC1101_GetState(void) {
    return CC1101_ReadStatus(0x35) & 0x1F; // Mask MARCSTATE
}

uint8_t CC1101_GetRXBytes(void) {
    return CC1101_ReadStatus(0x3B) & 0x7F;
}

uint8_t CC1101_GetTXBytes(void) {
    return CC1101_ReadStatus(0x3A) & 0x7F;
}

void debug_radio_state(void) {
    char buff[64];
    uint8_t status = CC1101_GetState();
    uint8_t tx_bytes = CC1101_GetTXBytes();
    uint8_t rx_bytes = CC1101_GetRXBytes(); // Fixed missing semicolon

    int len = snprintf(buff, sizeof(buff),
        "State: %u | TX: %u | RX: %u\r\n", status, tx_bytes, rx_bytes);
    HAL_UART_Transmit(&huart4, (uint8_t*)buff, (uint16_t)len, 100);
}
