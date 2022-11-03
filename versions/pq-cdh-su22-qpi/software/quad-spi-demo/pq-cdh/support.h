// support.h
// Tartan Artibeus CDH board offchip-flash support file
//
// Written by Bradley Denby
// Other contributors: Saral -> SPI/Flash interface
//
// See the top-level LICENSE file for the license.


#ifndef STM32L4
#define STM32L4
#endif

#ifndef SUPPORT_H
#define SUPPORT_H
// Initialization functions

void blinkLedBlocking(void);
void blinkLedBrief(void);

void init_clock(void);
void init_led(void);
void init_uart(void);
void init_quad_spi(void);

void flash_readProductInformation(uint8_t *mfrId, uint8_t *memory, uint8_t *capacity);
uint8_t flash_readStatusRegister(void);
void flash_writeData(uint8_t *dataBuffer, uint8_t bytesToWrite, uint32_t address);
void flash_readData(uint8_t *dataBuffer, uint8_t bytesToRead, uint32_t address);

// TODO: Implement these functions
// void testFlash(void); // Nuke the entire flash and write a test pattern, then read it back

enum flash_status_E{
    flash_status_undefined = 0,
    flash_status_failed,
    flash_status_success
};

#endif
