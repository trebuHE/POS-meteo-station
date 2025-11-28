#include "CC1101_BSP.h"

// --- Helper Functions ---

void CC1101_Select(void) {
    HAL_GPIO_WritePin(CC1101_CSN_PORT, CC1101_CSN_PIN, GPIO_PIN_RESET);
}

void CC1101_Deselect(void) {
    HAL_GPIO_WritePin(CC1101_CSN_PORT, CC1101_CSN_PIN, GPIO_PIN_SET);
}

void CC1101_WriteBurst(uint8_t reg, uint8_t *buf, uint8_t len) {
    uint8_t addr = reg | CC1101_WRITE_BYTE | CC1101_BURST_BIT;
    CC1101_Select();
    HAL_SPI_Transmit(&CC1101_SPI_HANDLE, &addr, 1, 100);
    HAL_SPI_Transmit(&CC1101_SPI_HANDLE, buf, len, 100);
    CC1101_Deselect();
}

void CC1101_ReadBurst(uint8_t reg, uint8_t *buf, uint8_t len) {
    uint8_t addr = reg | CC1101_READ_BYTE | CC1101_BURST_BIT;
    CC1101_Select();
    HAL_SPI_Transmit(&CC1101_SPI_HANDLE, &addr, 1, 100);
    HAL_SPI_Receive(&CC1101_SPI_HANDLE, buf, len, 100);
    CC1101_Deselect();
}

// --- Basic Primitives ---

void CC1101_CommandStrobe(uint8_t cmd) {
    CC1101_Select();
    HAL_SPI_Transmit(&CC1101_SPI_HANDLE, &cmd, 1, 100);
    CC1101_Deselect();
}

void CC1101_WriteReg(uint8_t reg, uint8_t value) {
    uint8_t data[2];
    data[0] = reg | CC1101_WRITE_BYTE;
    data[1] = value;
    CC1101_Select();
    HAL_SPI_Transmit(&CC1101_SPI_HANDLE, data, 2, 100);
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

// Settings for 433 MHz, GFSK, 4.8 kBaud, Variable Packet Length
void CC1101_Config(void) {
    // GDO0 asserts when Sync Word received, de-asserts at end of packet (0x06)
    CC1101_WriteReg(CC1101_IOCFG0, 0x06);
    CC1101_WriteReg(CC1101_IOCFG2, 0x06);

    // FIFO Threshold
    CC1101_WriteReg(CC1101_FIFOTHR, 0x47);

    // Packet Control: Variable length, CRC enabled, Check Addr(No)
    CC1101_WriteReg(CC1101_PKTCTRL0, 0x05); // 0x05: Variable len, CRC En
    CC1101_WriteReg(CC1101_PKTCTRL1, 0x04); // Append Status bytes

    // Frequency Setup (433.92 MHz)
    CC1101_WriteReg(CC1101_FREQ2, 0x10);
    CC1101_WriteReg(CC1101_FREQ1, 0xB1);
    CC1101_WriteReg(CC1101_FREQ0, 0x3B);

    // Modem Setup (4.8 kBaud, GFSK)
    CC1101_WriteReg(CC1101_MDMCFG4, 0xF5);
    CC1101_WriteReg(CC1101_MDMCFG3, 0x83);
    CC1101_WriteReg(CC1101_MDMCFG2, 0x11); // GFSK, Sync Mode 30/32

    CC1101_WriteReg(CC1101_DEVIATN, 0x15); // 5.15 kHz deviation

    // MCSM0: Auto calibrate on IDLE->RX/TX. Remain in RX after RX.
    CC1101_WriteReg(CC1101_MCSM0, 0x18);

    CC1101_WriteReg(CC1101_FOCCFG, 0x16);
    CC1101_WriteReg(CC1101_WORCTRL, 0xFB);

    // PATABLE (0 dBm)
    // 0xC0 = +10 dBm (Too strong for desk testing)
    // 0x60 = 0 dBm   (Good for desk testing)
    // 0x12 = -30 dBm (Use if still crashing at close range)
    uint8_t paTable[] = {0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    CC1101_WriteBurst(CC1101_PATABLE, paTable, 8);
}

// --- Init ---
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

    // Verify Chip
    if (CC1101_ReadStatus(CC1101_PARTNUM) != CC1101_PARTNUM_VAL) return 0;
    if (CC1101_ReadStatus(CC1101_VERSION) != CC1101_VERSION_VAL) return 0;

    // Apply Config
    CC1101_Config();

    return 1;
}

// --- Transmission ---

// Sends a packet. Packet structure: [Length Byte] [Data...]
void CC1101_Transmit(uint8_t *data, uint8_t length) {
    CC1101_CommandStrobe(CC1101_SIDLE);  // Idle
    CC1101_CommandStrobe(CC1101_SFTX);   // Flush TX FIFO

    // Fill FIFO: [Length] [Data]
    CC1101_WriteReg(CC1101_FIFO, length);
    CC1101_WriteBurst(CC1101_FIFO, data, length);

    CC1101_CommandStrobe(CC1101_STX);    // Start Transmit

    // Wait for GDO0 to de-assert (Sync word sent + Packet sent)
    // Simple blocking wait (optional, or use interrupt in real app)
    while (HAL_GPIO_ReadPin(CC1101_GDO0_PORT, CC1101_GDO0_PIN) == GPIO_PIN_RESET); // Wait for Sync
    while (HAL_GPIO_ReadPin(CC1101_GDO0_PORT, CC1101_GDO0_PIN) == GPIO_PIN_SET);   // Wait for End

    CC1101_CommandStrobe(CC1101_SIDLE); // Back to Idle
}

// --- Reception ---

// Put radio in RX mode
void CC1101_SetRx(void) {
    CC1101_CommandStrobe(CC1101_SRX);
}

// Check for packet and read if available
// Returns: length of payload read, or 0 if no packet
uint8_t CC1101_Receive(uint8_t *data) {
	uint8_t rxBytes = CC1101_ReadStatus(CC1101_RXBYTES) & 0x7F;

	    // Check if we have at least the Length byte
	    if (rxBytes > 1) {

	        // 1. Read the Length Byte
	        uint8_t len = CC1101_ReadReg(CC1101_FIFO);

	        // Safety: If length is huge (garbage), flush and exit
	        if (len > 60) {
	            CC1101_CommandStrobe(CC1101_SFRX);
	            CC1101_SetRx();
	            return 0;
	        }

	        // 2. WAIT for the rest of the packet to arrive
	        // We need 'len' bytes + 2 Status bytes
	        uint8_t bytesNeeded = len + 2;
	        uint32_t timeout = HAL_GetTick() + 500; // 500ms timeout

	        while (1) {
	            // Check how many bytes are currently in FIFO
	            rxBytes = CC1101_ReadStatus(CC1101_RXBYTES) & 0x7F;

	            if (rxBytes >= bytesNeeded) {
	                break; // All data is here!
	            }

	            if (HAL_GetTick() > timeout) {
	                // Timeout waiting for packet completion
	                CC1101_CommandStrobe(CC1101_SFRX);
	                CC1101_SetRx();
	                return 0;
	            }

	            // Wait a bit to let radio receive
	            HAL_Delay(2);
	        }

	        // 3. Now it is safe to read the payload
	        CC1101_ReadBurst(CC1101_FIFO, data, len);

	        // 4. Read (and discard) the 2 status bytes (RSSI/LQI)
	        uint8_t status[2];
	        CC1101_ReadBurst(CC1101_FIFO, status, 2);

	        // 5. Check CRC (Optional but recommended)
	        // The last bit of the 2nd status byte is CRC_OK
	        if (!(status[1] & 0x80)) {
	            // CRC Failed - Packet is corrupt
	            CC1101_SetRx();
	            return 0;
	        }

	        CC1101_SetRx();
	        return len;
	    }
	    return 0;
}

uint8_t CC1101_GetState(void) {
    uint8_t status = CC1101_ReadStatus(0x35); // 0x35 is MARCSTATE
    return status & 0x1F; // Mask out the upper bits
}

// --- Robust Packet Receive using GDO0 ---
// precise, prevents reading garbage/partial packets
uint8_t CC1101_Receive_Strict(uint8_t *data) {

    // 1. Check GDO0 (Pin A0)
    // If GDO0 is High, a packet is currently being received.
    // We wait for it to drop to Low. That marks the exact end of the packet.
    if (HAL_GPIO_ReadPin(CC1101_GDO0_PORT, CC1101_GDO0_PIN) == GPIO_PIN_SET) {

        // Wait for pin to go Low (Packet End)
        // Add a timeout to prevent hanging if it gets stuck
        uint32_t t = HAL_GetTick();
        while (HAL_GPIO_ReadPin(CC1101_GDO0_PORT, CC1101_GDO0_PIN) == GPIO_PIN_SET) {
            if (HAL_GetTick() - t > 100) return 0; // Timeout
        }

        // 2. Now we know a packet is fully in the FIFO. Read it.
        uint8_t rxBytes = CC1101_ReadStatus(CC1101_RXBYTES) & 0x7F;

        // Safety check: Do we have data?
        if (rxBytes > 1) {
            uint8_t len = CC1101_ReadReg(CC1101_FIFO);

            // Safety Check: Payload too large?
            if (len > 60) {
                CC1101_CommandStrobe(CC1101_SFRX); // Flush
                CC1101_SetRx();
                return 0;
            }

            // Read Data
            CC1101_ReadBurst(CC1101_FIFO, data, len);

            // Read & Discard Status Bytes (RSSI + LQI)
            // Essential to keep FIFO alignment!
            uint8_t status[2];
            CC1101_ReadBurst(CC1101_FIFO, status, 2);

            // Optional: Check CRC (Bit 7 of LQI)
            if (!(status[1] & 0x80)) {
                // CRC Error
                CC1101_SetRx();
                return 0;
            }

            CC1101_SetRx();
            return len;
        }
    }

    // If GDO0 was Low, no packet is arriving right now.
    // Make sure we are in RX mode
    if (CC1101_GetState() != 0x0D) {
        CC1101_SetRx();
    }

    return 0;
}
