// support.c
// Tartan Artibeus CDH board offchip-flash support file
//
// Written by Bradley Denby
// Other contributors: Saral -> SPI/Flash interface
//
// Relevant datasheets: https://www.issi.com/WW/pdf/25LP-WP128F.pdf
//
// See the top-level LICENSE file for the license.

// Standard library
#include <stdint.h> // uint8_t
#include <assert.h>
#include </Users/saraltayal/Desktop/pocketqube/pq-cdh-su22/software/spi-demo/pq-cdh/support.h>

// libopencm3 library
#include <libopencm3/cm3/scb.h>     // SCB_VTOR
#include <libopencm3/stm32/flash.h> // used in init_clock
#include <libopencm3/stm32/gpio.h>  // used in init_gpio
#include <libopencm3/stm32/rcc.h>   // used in init_clock
#include <libopencm3/stm32/usart.h> // used in init_uart
#include <libopencm3/stm32/spi.h>   // used in the SPI interface

/*************************************************/
/*               Private variables               */    
/*************************************************/

#define NULL 0

/* Please don't refactor to #defs for these since they're size sensitive */

/**** Register and Instruction addresses ****/
static const uint8_t flash_statusRegister_id = 0x05;
static const uint8_t flash_flashInformation_id = 0x9f;
static const uint8_t flash_pageProgram_id = 0x02;
static const uint8_t flash_readPage_id = 0x03;
static const uint8_t flash_writeEnable_id = 0x06;

/**** Supporting Constants ****/
static const uint8_t flash_dummyBytes = 0x00;
static const uint8_t flash_writeCompleteMask = 0x01;

// TODO: Confirm flash size consts
// TODO: Refactor datasizes to be more representative of the actual data
static const uint32_t flash_capacity = 0x18;
static const uint32_t flash_numSectors = 4096;
static const uint32_t flash_sectorSize = 4096;
static const uint32_t flash_sectorShift = 12; // 2^12 = 4096
static const uint32_t flash_pageSize = 256;
static const uint32_t flash_pageShift = 8;  // 2^8 = 256
static const uint32_t flash_numPages = 65536;
static const uint32_t flash_maxAddress = 0x00FFFFFF;

/*************************************************/
/*               Private functions               */    
/*************************************************/

static int16_t swap16(int16_t val)
{
  return (val << 8) | ((val >> 8) & 0xFF);
}

static uint8_t flash_xfer8(uint8_t data){
  // correct way
  // 1. Wait for TXE 
  // 2. Write data to DR
  // 3. Wait for RXNE
  // 4. Read data from DR
  while((SPI_SR(SPI1) & SPI_SR_TXE) == 0);
  SPI_DR(SPI1) = data;
  while((SPI_SR(SPI1) & SPI_SR_RXNE) == 0);
  return SPI_DR(SPI1);
  // SPI_DR8(SPI1) = data;
  // while(!(SPI_SR(SPI1) & SPI_SR_RXNE));
  // return SPI_DR8(SPI1);
}

static void flash_write8(uint8_t data){
  while(!(SPI_SR(SPI1) & SPI_SR_TXE));
  SPI_DR8(SPI1) = data;
  while(!(SPI_SR(SPI1) & SPI_SR_TXE));
}

static uint16_t flash_xfer16(uint16_t data){
  uint16_t data_read = spi_xfer(SPI1, swap16(data));
  return data_read;
}

static void flash_send8(uint8_t data){
  // while (!(SPI_SR(SPI1) & SPI_SR_TXE));
  spi_send8(SPI1, data); 
  // while (!(SPI_SR(SPI1) & SPI_SR_TXE));
}

// static void flash_send16(uint16_t data){
//   spi_send(SPI1, swap16(data)); 
// }

static uint8_t flash_read8(void){
  uint8_t data_read = spi_read8(SPI1);
  return data_read;
}

static bool flash_writeComplete(void){
  uint8_t status = flash_readStatusRegister();
  return ((status & flash_writeCompleteMask) == 0x00);
}

/*************************************************/
/*              Misc Init functions              */    
/*************************************************/

// Initialization functions
void init_clock(void)
{
  rcc_osc_on(RCC_HSI16);                    // 16 MHz internal RC oscillator
  rcc_wait_for_osc_ready(RCC_HSI16);        // Wait until oscillator is ready
  rcc_set_sysclk_source(RCC_CFGR_SW_HSI16); // Sets sysclk source for RTOS
  rcc_set_hpre(RCC_CFGR_HPRE_NODIV);        // AHB at 80 MHz (80 MHz max.)
  rcc_set_ppre1(RCC_CFGR_PPRE_DIV2);        // APB1 at 40 MHz (80 MHz max.)
  rcc_set_ppre2(RCC_CFGR_PPRE_NODIV);       // APB2 at 80 MHz (80 MHz max.)
  // flash_prefetch_enable();                  // Enable instr prefetch buffer
  flash_set_ws(FLASH_ACR_LATENCY_4WS); // RM0351: 4 WS for 80 MHz
  // flash_dcache_enable();                    // Enable data cache
  // flash_icache_enable();                    // Enable instruction cache
  rcc_set_main_pll(                       // Setup 80 MHz clock
      RCC_PLLCFGR_PLLSRC_HSI16,           // PLL clock source
      4,                                  // PLL VCO division factor
      40,                                 // PLL VCO multiplication factor
      0,                                  // PLL P clk output division factor
      0,                                  // PLL Q clk output division factor
      RCC_PLLCFGR_PLLR_DIV2               // PLL sysclk output division factor
  );                                      // 16MHz/4 = 4MHz; 4MHz*40 = 160MHz VCO; 160MHz/2 = 80MHz PLL
  rcc_osc_on(RCC_PLL);                    // 80 MHz PLL
  rcc_wait_for_osc_ready(RCC_PLL);        // Wait until PLL is ready
  rcc_set_sysclk_source(RCC_CFGR_SW_PLL); // Sets sysclk source for RTOS
  rcc_wait_for_sysclk_status(RCC_PLL);
  rcc_ahb_frequency = 80000000;
  rcc_apb1_frequency = 40000000;
  rcc_apb2_frequency = 80000000;
}

void init_led(void)
{
  rcc_periph_clock_enable(RCC_GPIOC);
  gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO10);
  gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);
  gpio_set(GPIOC, GPIO10);
  gpio_clear(GPIOC, GPIO12);
}

void init_uart(void)
{
  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_USART1);
  gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9 | GPIO10);
  gpio_set_af(GPIOA, GPIO_AF7, GPIO9);  // USART1_TX is alternate function 7
  gpio_set_af(GPIOA, GPIO_AF7, GPIO10); // USART1_RX is alternate function 7
  usart_set_baudrate(USART1, 115200);
  usart_set_databits(USART1, 8);
  usart_set_stopbits(USART1, USART_STOPBITS_1);
  usart_set_mode(USART1, USART_MODE_TX_RX);
  usart_set_parity(USART1, USART_PARITY_NONE);
  usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
  usart_enable(USART1);
}

/*************************************************/
/*               Public Functions                */    
/*************************************************/

void blinkLedBlocking()
{
  while (1)
  {
    gpio_clear(GPIOB, GPIO0);
    gpio_toggle(GPIOC, GPIO10);
    for (int i = 0; i < 8000000; i++)
    {
      __asm__("nop");
    }
  }
}

/*************************************************/
/*            SPI & Flash Interface              */    
/*************************************************/

// Datasheet: https://www.issi.com/WW/pdf/25LP-WP128F.pdf

void init_spi(){

  /**SPI1 GPIO Configuration
  PB0 ------> SPI1_NSS
  PB3 ------> SPI1_SCK
  PB4 ------> SPI1_MISO
  PB5 ------> SPI1_MOSI
  */

  rcc_periph_clock_enable(RCC_SPI1);
  rcc_periph_clock_enable(RCC_GPIOB);
  gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO3 | GPIO4 | GPIO5);
  gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO0);
  gpio_set_af(GPIOB, GPIO_AF5, GPIO3 | GPIO4 | GPIO5);
  gpio_set(GPIOB, GPIO0); // set the Chip select line to high since it's active low

  spi_reset(SPI1);
  
  // One can use either mode 1 or 3 (so cpol = 0, cpha = 1 or cpol = 1, cpha = 2)
  spi_init_master(
      SPI1,
      SPI_CR1_BAUDRATE_FPCLK_DIV_64,
      SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
      SPI_CR1_CPHA_CLK_TRANSITION_1,
      SPI_CR1_MSBFIRST);

  spi_enable_software_slave_management(SPI1);
  spi_set_nss_high(SPI1);
  spi_enable(SPI1);

  // check to see if peripheral is enabled
  while (!(SPI_SR(SPI1) & SPI_SR_TXE));

  // reset the IC, recomended per the datasheet
  gpio_clear(GPIOB, GPIO0);
  gpio_set(GPIOB, GPIO0);

}

void flash_readProductInformation(uint8_t *mfrId, uint8_t *memory, uint8_t *capacity){

  if (mfrId == NULL || memory == NULL || capacity == NULL){
    return; // TODO: Add error handling
  }

  *mfrId = 0;
  *memory = 0;
  *capacity = 0;

  gpio_clear(GPIOB, GPIO0);

  flash_send8(flash_flashInformation_id); 
  flash_xfer8(flash_dummyBytes);
  *mfrId = flash_xfer8(flash_dummyBytes);
  *memory = flash_xfer8(flash_dummyBytes);
  *capacity = flash_xfer8(flash_dummyBytes);

  assert(*mfrId == 0x9d);
  assert(*memory == 0x60);
  assert(*capacity == 0x18);

  gpio_set(GPIOB, GPIO0);

  return;
}

uint8_t flash_readStatusRegister(){
  uint8_t status = 0xFF;

  gpio_clear(GPIOB, GPIO0);
  flash_send8(flash_statusRegister_id);
  do{
    status = flash_xfer8(flash_statusRegister_id);
  }while (status != 0x00);
  gpio_set(GPIOB, GPIO0);

  return status;
}

void flash_writeData(uint8_t *dataBuffer, uint8_t bytesToWrite, uint32_t address){
  
  if (dataBuffer == NULL || bytesToWrite == 0 || address > flash_maxAddress){
    return; // TODO: Add error handling
  }

  // wait for write complete -- check status register
  flash_readStatusRegister();

  // Write enable
  gpio_clear(GPIOB, GPIO0);
  flash_xfer8(flash_writeEnable_id);
  while (!(SPI_SR(SPI1) & SPI_SR_TXE));
  gpio_set(GPIOB, GPIO0);

  // TODO: do math to figure out what page we need to write to

  gpio_clear(GPIOB, GPIO0);
  flash_xfer8(flash_pageProgram_id);
  flash_xfer8((address >> 24) && 0xFF);
  flash_xfer8((address >> 16) && 0xFF);
  flash_xfer8((address >>  8) && 0xFF);
  flash_xfer8((address >>  0) && 0xFF);
  
  uint8_t bytesWritten_internal = 0;
  while((bytesToWrite - bytesWritten_internal) > 0){
    flash_xfer8(dataBuffer[bytesWritten_internal++]);
  }
  gpio_set(GPIOB, GPIO0);

  return;
}


void flash_readData(uint8_t *dataBuffer, uint8_t bytesToRead, uint32_t address){
  if (dataBuffer == NULL || bytesToRead == 0){
    return; // TODO: Add error handling
  }

  // wait for write complete -- check status register
  flash_readStatusRegister();

  gpio_clear(GPIOB, GPIO0);
  flash_xfer8(flash_readPage_id);
  flash_xfer8((address >> 24) && 0xFF);
  flash_xfer8((address >> 16) && 0xFF);
  flash_xfer8((address >>  8) && 0xFF);
  flash_xfer8((address >>  0) && 0xFF);

  flash_xfer8(flash_dummyBytes);  
  flash_xfer8(flash_dummyBytes);  
  
  uint8_t bytesRead_internal = 0;
  while((bytesToRead - bytesRead_internal) > 0){
    dataBuffer[bytesRead_internal++] = flash_xfer8(flash_dummyBytes);
  }

  gpio_clear(GPIOB, GPIO0);


  return;

}
