#include <libopencm3/stm32/l4/quadspi.h>

/* Local Defines */
#define QUADSPI_CR_PRESCALER_BYPASS  0x01
#define QUADSPI_CR_PRESCALER_DIV_2   0x01
#define QUADSPI_CR_PRESCALER_DIV_4   0x03
#define QUADSPI_CR_PRESCALER_DIV_16  0x0e
#define QUADSPI_CR_PRESCALER_DIV_32  0x40

#define QUADSPI_CR_FTHRES_00         0x00

#define FLASH_SIZE_POW_2             16  // 2^16 = 64KB
#define FLASH_ENTER_QUAD_MODE        0x35
#define FLASH_WRITE_ENABLE           0x06
#define FLASH_QUAD_READ              0xED
#define FLASH_PAGE_PROGRAM           0x02
#define FLASH_READ_STATUS_REG        0x05

#define QUADSPI_CCR_ADDRESS_24BIT    0b10


/* Temp forward declarations */
void init_qspi(void);
void qspi_writeInstruction(uint8_t instruction);
void qspi_writeData(uint8_t *data, uint32_t data_size, uint32_t address);
void qspi_readData(uint8_t *data, uint32_t data_size, uint32_t address);
void qspi_readDataNoAddress(uint8_t *dataBuffer, uint32_t dataBuffer_size, uint8_t instruction);
void qspi_readDataWithAddress(uint8_t *dataBuffer, uint32_t dataBuffer_size, uint32_t address);
void qspi_waitForChip();


// TODO: Dummy cycles


/*
 * Quad SPI
 * Dual flash mode disabled
 * Indirect mode (for both reads and writes)
 * QSPI configuration, page 496 of the stm32l4 reference manual 
 */
void init_qspi(){
  /**QSPI1 GPIO Configuration
  PB1 ------> IO0
  PB0 ------> IO1
  PA7 ------> IO2
  PA6 ------> IO3
  PA3 ------> CLK
  PA2 ------> NSS
  */

  rcc_periph_clock_enable(RCC_QSPI);
  rcc_periph_clock_enable(RCC_GPIOB);
  rcc_periph_clock_enable(RCC_GPIOA);
  gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO1 | GPIO0);
  gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2 | GPIO3 | GPIO6 | GPIO7);
  gpio_set_af(GPIOA, GPIO_AF10, GPIO3 | GPIO2 | GPIO6 | GPIO7);
  // gpio_set_af(GPIOA, GPIO_AF10, GPIO2 | GPIO3 | GPIO7);
  // gpio_set_af(GPIOA, GPIO_AF10, GPIO6);
  gpio_set_af(GPIOB, GPIO_AF10, GPIO1 | GPIO0);

  // gpio_set(GPIOA, GPIO2); // set the Chip select line to high since it's active low

  // // reset the peripheral 
  // gpio_set(GPIOA, GPIO2); 

  while(QUADSPI_SR & QUADSPI_SR_BUSY);

  // Note: Can only be set when BUSY = 0

  // Set the clock prescaler division factor in CR
  QUADSPI_CR = (QUADSPI_CR & ~QUADSPI_CR_PRESCALE_MASK) | (QUADSPI_CR_PRESCALER_DIV_16 << QUADSPI_CR_PRESCALE_SHIFT); 

  // set the flash size
  QUADSPI_DCR |= (FLASH_SIZE_POW_2 << QUADSPI_DCR_FSIZE_SHIFT);

  // // set address size to 24 bits
  // // initially set instructions to 1 line to configure the flash to qspi mode
  // QUADSPI_CCR |= (QUADSPI_CCR_ADDRESS_24BIT << QUADSPI_CCR_ADSIZE_SHIFT) | 
  //                (QUADSPI_CCR_MODE_1LINE << QUADSPI_CCR_IMODE_SHIFT);

  // Specify the instruction mode via the CCR (use 4 lines for instructions)
  QUADSPI_CCR &= ~(QUADSPI_CCR_IMODE_MASK  |
                   QUADSPI_CCR_ADMODE_MASK |
                   QUADSPI_CCR_DMODE_MASK  |
                   QUADSPI_CCR_INST_MASK);

  QUADSPI_CCR |= (QUADSPI_CCR_MODE_1LINE << QUADSPI_CCR_IMODE_SHIFT) |
                 (QUADSPI_CCR_MODE_NONE << QUADSPI_CCR_ADMODE_SHIFT) |
                 (QUADSPI_CCR_ADDRESS_24BIT << QUADSPI_CCR_ADSIZE_SHIFT) |
                 (QUADSPI_CCR_MODE_NONE << QUADSPI_CCR_DMODE_SHIFT);
  
  QUADSPI_CCR = (QUADSPI_CCR & ~QUADSPI_CCR_FMODE_MASK) | (QUADSPI_CCR_FMODE_IWRITE <<  QUADSPI_CCR_FMODE_SHIFT);

  // Set the Sample Shifting Settings in CR. Setup to read half a clock cycle later
  QUADSPI_CR |= QUADSPI_CR_SSHIFT;

  // Enable QSPI via the EN bit in the CR register 
  QUADSPI_CR |= QUADSPI_CR_EN;

  // Tell the flash to enter quad mode
  QUADSPI_CCR |= (FLASH_ENTER_QUAD_MODE << QUADSPI_CCR_INST_SHIFT);

  while(QUADSPI_SR & QUADSPI_SR_BUSY);
  while(!(QUADSPI_SR & QUADSPI_SR_TCF));

  // Disable the peripheral
  QUADSPI_CR &= ~QUADSPI_CR_EN;

  // tell the flash to enter quad mode
  // gpio_set_af(GPIOA, GPIO_AF10, GPIO6);
  while(QUADSPI_SR & QUADSPI_SR_BUSY);
  blinkLedBrief();

  qspi_writeInstruction(FLASH_WRITE_ENABLE);

  // qspi_waitForChip();
}

void qspi_waitForChip(){
  uint8_t status = 0;
  do{
    qspi_readDataNoAddress(&status, 1, FLASH_READ_STATUS_REG);
  // }while(status & 0x01);
  }while(false);
}

void qspi_writeInstruction(uint8_t instruction){
  // wait for the bus to be ready
  while(QUADSPI_SR & QUADSPI_SR_BUSY);

  // Set FMODE to 00 for Indirect Write Mode
  QUADSPI_CCR = (QUADSPI_CCR & ~QUADSPI_CCR_FMODE_MASK) | (QUADSPI_CCR_FMODE_IWRITE <<  QUADSPI_CCR_FMODE_SHIFT);

  // No data to write, set DLR to 0
  QUADSPI_DLR = 0;

  // Specify the instruction mode via the CCR (use 4 lines for instructions)
  QUADSPI_CCR &= ~(QUADSPI_CCR_IMODE_MASK  |
                   QUADSPI_CCR_ADMODE_MASK |
                   QUADSPI_CCR_DMODE_MASK  |
                   QUADSPI_CCR_INST_MASK);

  QUADSPI_CCR |= (QUADSPI_CCR_MODE_4LINE << QUADSPI_CCR_IMODE_SHIFT) |
                 (QUADSPI_CCR_MODE_NONE << QUADSPI_CCR_ADMODE_SHIFT) |
                 (QUADSPI_CCR_MODE_NONE << QUADSPI_CCR_DMODE_SHIFT);

  // Enable the peripheral via the CR
  QUADSPI_CR |= QUADSPI_CR_EN;

  // setup the the instructions and address
  QUADSPI_CCR |= (instruction << QUADSPI_CCR_INST_SHIFT);

  // wait till the transfer is complete
  while(!(QUADSPI_SR & QUADSPI_SR_TCF));
  while(QUADSPI_SR & QUADSPI_SR_BUSY);

  // Disable the peripheral via the CR
  QUADSPI_CR &= ~QUADSPI_CR_EN;
}

void qspi_writeData(uint8_t *dataBuffer, uint32_t dataBuffer_size, uint32_t address){

  
  while(QUADSPI_SR & QUADSPI_SR_BUSY);

  // Set FMODE to 00 for Indirect Write Mode
  QUADSPI_CCR = (QUADSPI_CCR & ~QUADSPI_CCR_FMODE_MASK) | (QUADSPI_CCR_FMODE_IWRITE <<  QUADSPI_CCR_FMODE_SHIFT);


  // Specify the instruction mode via the CCR (use 4 lines for instructions)
  QUADSPI_CCR &= ~(QUADSPI_CCR_IMODE_MASK  |
                   QUADSPI_CCR_ADMODE_MASK |
                   QUADSPI_CCR_DMODE_MASK  |
                   QUADSPI_CCR_INST_MASK);

  QUADSPI_CCR |= (QUADSPI_CCR_MODE_4LINE << QUADSPI_CCR_IMODE_SHIFT) |
                 (QUADSPI_CCR_MODE_4LINE << QUADSPI_CCR_ADMODE_SHIFT) |
                 (QUADSPI_CCR_MODE_4LINE << QUADSPI_CCR_DMODE_SHIFT);

  
  // Set the number of bytes to write via the DLR (data length register)
  //@note: hardcoding to 1 bytes for now
  QUADSPI_DLR = (uint32_t) 0x0; // NOTE: 0x0 is 1 byte, 0x1 is 2 bytes, etc.

  // Enable the peripheral via the CR
  QUADSPI_CR |= QUADSPI_CR_EN;

  QUADSPI_CCR |= (FLASH_PAGE_PROGRAM << QUADSPI_CCR_INST_SHIFT);
  QUADSPI_AR = address;
  while((QUADSPI_SR & QUADSPI_SR_TCF) == 0);
  QUADSPI_DR = (uint32_t) 0x000000ff; // TODO: Temp

  // wait till the transfer is complete
  while(QUADSPI_SR & QUADSPI_SR_BUSY);
  QUADSPI_DR = (uint32_t) 0x000000ff; // TODO: Temp

  // Disable the peripheral via the CR
  QUADSPI_CR &= ~QUADSPI_CR_EN;
}

void qspi_readDataNoAddress(uint8_t *dataBuffer, uint32_t dataBuffer_size, uint8_t instruction){
  // wait for the bus to be ready
  while(QUADSPI_SR & QUADSPI_SR_BUSY);

  // Set FMODE to 01 for Indirect Read Mode
  QUADSPI_CCR = (QUADSPI_CCR & ~QUADSPI_CCR_FMODE_MASK) | (QUADSPI_CCR_FMODE_IREAD <<  QUADSPI_CCR_FMODE_SHIFT);

  // Specify the instruction mode via the CCR (use 4 lines for instructions)
  QUADSPI_CCR &= ~(QUADSPI_CCR_IMODE_MASK  |
                   QUADSPI_CCR_ADMODE_MASK |
                   QUADSPI_CCR_DMODE_MASK  |
                   QUADSPI_CCR_INST_MASK);

  QUADSPI_CCR |= (QUADSPI_CCR_MODE_4LINE << QUADSPI_CCR_IMODE_SHIFT) |
                 (QUADSPI_CCR_MODE_NONE << QUADSPI_CCR_ADMODE_SHIFT) |
                 (QUADSPI_CCR_MODE_4LINE << QUADSPI_CCR_DMODE_SHIFT);

  // Set the number of bytes to read via the DLR (data length register)
  //@note: hardcoding to 1 bytes for now
  QUADSPI_DLR = (uint32_t) 0x0; // NOTE: 0x0 is 1 byte, 0x1 is 2 bytes, etc.
                  
  // Enable the peripheral via the CR
  QUADSPI_CR |= QUADSPI_CR_EN;

  // setup the the read instruction
  QUADSPI_CCR |= (instruction << QUADSPI_CCR_INST_SHIFT);

  // wait till the transfer is complete
  while(!(QUADSPI_SR & QUADSPI_SR_TCF));
  dataBuffer[0] = (uint8_t) (QUADSPI_DR && (uint32_t) 0xff);
  while(QUADSPI_SR & QUADSPI_SR_BUSY);

  //@note: Hardcoding to 1 byte for now
  // Get RX data from the DR

  // Disable the peripheral
  QUADSPI_CR &= ~QUADSPI_CR_EN;

}

void qspi_readDataWithAddress(uint8_t *dataBuffer, uint32_t dataBuffer_size, uint32_t address){

  // wait for the bus to be ready
  while(QUADSPI_SR & QUADSPI_SR_BUSY);

  // Set FMODE to 01 for Indirect Read Mode
  QUADSPI_CCR = (QUADSPI_CCR & ~QUADSPI_CCR_FMODE_MASK) | (QUADSPI_CCR_FMODE_IREAD <<  QUADSPI_CCR_FMODE_SHIFT);

  // Specify the instruction mode via the CCR (use 4 lines for instructions)
  QUADSPI_CCR &= ~(QUADSPI_CCR_IMODE_MASK  |
                   QUADSPI_CCR_ADMODE_MASK |
                   QUADSPI_CCR_DMODE_MASK  |
                   QUADSPI_CCR_INST_MASK);

  QUADSPI_CCR |= (QUADSPI_CCR_MODE_4LINE << QUADSPI_CCR_IMODE_SHIFT) |
                 (QUADSPI_CCR_MODE_4LINE << QUADSPI_CCR_ADMODE_SHIFT) |
                 (QUADSPI_CCR_MODE_4LINE << QUADSPI_CCR_DMODE_SHIFT);

  // Set the number of bytes to write via the DLR (data length register)
  //@note: hardcoding to 1 bytes for now
  QUADSPI_DLR = (uint32_t) 0xa; // NOTE: 0x0 is 1 byte, 0x1 is 2 bytes, etc.
                  
  // Enable the peripheral via the CR
  QUADSPI_CR |= QUADSPI_CR_EN;

  // setup the the read instruction
  QUADSPI_CCR |= (FLASH_QUAD_READ << QUADSPI_CCR_INST_SHIFT);
  QUADSPI_AR = address;

  // wait till the transfer is complete
  for(int i = 0; i < 0xa; i++){
    while(!(QUADSPI_SR & QUADSPI_SR_TCF));
    dataBuffer[0] = QUADSPI_DR;
  }
  while(QUADSPI_SR & QUADSPI_SR_BUSY);

  //@note: Hardcoding to 1 byte for now
  // Get RX data from the DR
  dataBuffer[0] = QUADSPI_DR;

  // Disable the peripheral
  QUADSPI_CR &= ~QUADSPI_CR_EN;
}


/*
pi_writeData(uint8_t *dataBuffer, uint32_t dataBuffer_size, uint32_t address){
  // wait for the bus to be ready
  while(QUADSPI_SR & QUADSPI_SR_BUSY);

  // Set FMODE to 01 for Indirect Write Mode
  QUADSPI_CCR = (QUADSPI_CCR & ~QUADSPI_CCR_FMODE_MASK) | (QUADSPI_CCR_FMODE_IWRITE <<  & QUADSPI_CCR_FMODE_MASK);

  //@note: hardcoding to 1 bytes for now
  // Set the number of bytes to write via the DLR (data length register)
  QUADSPI_DLR = (uint32_t) 0x1;

  // Specify the frame format for the command phase via the CCR

  // Specify the instruction mode via the CCR (use 4 lines for instructions)
  QUADSPI_CCR = (QUADSPI_CCR & ~QUADSPI_CCR_IMODE_MASK) | (QUADSPI_CCR_IMODE_4_LINE << QUADSPI_CCR_IMODE_MASK);
  
  // Specify the address mode via the CCR (use 4 lines for addressing)
  QUADSPIT_CCR = (QUADSPI_CCR & ~QUADSPI_CCR_ADMODE_MASK) | (QUADSPI_CCR_MODE_4LINE << QUADSPI_CCR_ADMODE_SHIFT);

  // Specify the data mode via the CCR (use 4 lines for data)
  QUADSPI_CCR = (QUADSPI_CCR & ~QUADSPI_CCR_DMODE_MASK) | (QUADSPI_CCR_DMODE_4_LINE << QUADSPI_CCR_DMODE_MASK);

  // Optional: Set the Alternative byte via the ABR

  // Setup the CR DMA EN bit to disable DMA
  QUADSPI_CR &= ~QUADSPI_CR_DMAEN;

  // Setup the Timout counter enable to false
  QUADSPI_CR &= ~QUADSPI_CR_TCEN;

  // Setup the sample shifting to false
  QUADSPI_CR &= ~QUADSPI_CR_SSHIFT;

  // Setup the FIFO threshold to 0
  QUADSPI_CR = (QUADSPI_CR & ~QUADSPI_CR_FTHRES_MASK) | (QUADSPI_CR_FTHRES_00 << QUADSPI_CR_FTHRES_MASK);

  // Setup the Transfer complete interrupt enable to false
  QUADSPI_CR &= ~QUADSPI_CR_TCIE;

  // Setup the Transfer error interrupt enable to false
  QUADSPI_CR &= ~QUADSPI_CR_TEIE;

  // Set the Address via the AR
  QUADSPI_AR = address;

  //@note: Hardcoding to 1 byte for now
  // Set Data in the DR, also starts the communication
  QUADSPI_DR = dataBuffer[0];

  // wait till the transfer is complete
  while(!(QUADSPI_SR & QUADSPI_SR_TCF));
}  



*/