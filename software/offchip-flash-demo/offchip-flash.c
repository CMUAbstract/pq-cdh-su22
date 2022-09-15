//#include <stdio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/quadspi.h>
#include <libopencm3/stm32/spi.h>
//#include <libopencm3/stm32/dma.h>
//#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/dma.h>
#include </home/abstract/git-repos/pq-cdh-su22-master/software/offchip-flash-demo/pq-cdh/support.h>

//void init_clock(void);
//void init_led(void);
//void init_uart(void);
//void init_qspi(void);
void qspi_enable(uint8_t cmd);
void qspi_write_page(uint32_t addr, uint8_t *buf, int len);
uint32_t read_register(void);
void write_register(uint8_t value);


void qspi_enable(uint8_t cmd) {
	uint32_t ccr, sr;
  ccr = 0x0;
  ccr |= cmd;
  ccr |= (QUADSPI_CCR_MODE_4LINE << QUADSPI_CCR_IMODE_SHIFT);
  ccr |= (QUADSPI_CCR_FMODE_IWRITE << QUADSPI_CCR_FMODE_SHIFT);
  QUADSPI_CCR = ccr;
	do {
		sr = QUADSPI_SR;
	} while (sr & QUADSPI_SR_BUSY);
	QUADSPI_FCR = 0x1f; /* reset the flags */
}

uint32_t read_register() {
  uint32_t ccr, sr;
	uint32_t res = 0;

  ccr = 0x0;
  ccr |= 0x05;
	ccr |= (QUADSPI_CCR_FMODE_IREAD << QUADSPI_CCR_FMODE_SHIFT);
  ccr |= (QUADSPI_CCR_MODE_4LINE << QUADSPI_CCR_IMODE_SHIFT);
  ccr |= (QUADSPI_CCR_MODE_4LINE << QUADSPI_CCR_DMODE_SHIFT);
	QUADSPI_DLR = 0;	/* 8 bits */
	QUADSPI_CCR = ccr;
	do {
		res = QUADSPI_DR;
	} while (QUADSPI_SR & QUADSPI_SR_BUSY);
	QUADSPI_FCR = 0x1f;
	return (res & 0xff);
}

void write_register(uint8_t value) {
  uint32_t ccr;
	// uint32_t status;

	qspi_enable(0x06);
  // read_register();
  // usart_send_blocking(USART1, status);

  ccr = 0x0;
  ccr |= 0x01;
  ccr |= (QUADSPI_CCR_FMODE_IWRITE << QUADSPI_CCR_FMODE_SHIFT);
	ccr |= (QUADSPI_CCR_MODE_4LINE << QUADSPI_CCR_IMODE_SHIFT);
  ccr |= (QUADSPI_CCR_MODE_4LINE << QUADSPI_CCR_DMODE_SHIFT);
	QUADSPI_DLR = 0;	/* 8 bits */
	QUADSPI_CCR = ccr;
	QUADSPI_DR = value;
	while (QUADSPI_SR & QUADSPI_SR_BUSY);
	QUADSPI_FCR = 0x1f;
}

void qspi_write_page(uint32_t addr, uint8_t *buf, int len) {
	uint32_t ccr, sr;
	int tmp;

	qspi_enable(0x06);
  
  ccr = QUADSPI_CCR;

  ccr &= ~(QUADSPI_CCR_FMODE_MASK << QUADSPI_CCR_FMODE_SHIFT);
	ccr |= (QUADSPI_CCR_FMODE_IWRITE << QUADSPI_CCR_FMODE_SHIFT);
	/* adjusting this to 0 fixed the write issue. */
	
  // ccr |= QUADSPI_SET(CCR, DCYC, 0); 
  ccr &= ~(QUADSPI_CCR_DCYC_MASK << QUADSPI_CCR_DCYC_SHIFT);
	
  // ccr |= QUADSPI_SET(CCR, INST, 0x32);	/* write 256 bytes */
  ccr &= ~(QUADSPI_CCR_INST_MASK << QUADSPI_CCR_INST_SHIFT);
  ccr |= (0x32 << QUADSPI_CCR_INST_SHIFT);
	
  /* For some reason 1-1-4 command */
	// ccr |= QUADSPI_SET(CCR, IMODE, QUADSPI_CCR_MODE_1LINE);
  ccr &= ~(QUADSPI_CCR_IMODE_MASK << QUADSPI_CCR_IMODE_SHIFT);
	ccr |= (QUADSPI_CCR_MODE_1LINE << QUADSPI_CCR_IMODE_SHIFT);

	// ccr |= QUADSPI_SET(CCR, ADMODE, QUADSPI_CCR_MODE_1LINE);
  ccr &= ~(QUADSPI_CCR_ADMODE_MASK << QUADSPI_CCR_ADMODE_SHIFT);
	ccr |= (QUADSPI_CCR_MODE_1LINE << QUADSPI_CCR_ADMODE_SHIFT);

	// ccr |= QUADSPI_SET(CCR, ADSIZE, 2);	/* 24 bit address */
  ccr &= ~(QUADSPI_CCR_ADSIZE_MASK << QUADSPI_CCR_ADSIZE_SHIFT);
	ccr |= (0x2 << QUADSPI_CCR_ADSIZE_SHIFT);

	// ccr |= QUADSPI_SET(CCR, DMODE, QUADSPI_CCR_MODE_4LINE);
  ccr &= ~(QUADSPI_CCR_DMODE_MASK << QUADSPI_CCR_DMODE_SHIFT);
	ccr |= (QUADSPI_CCR_MODE_4LINE << QUADSPI_CCR_DMODE_SHIFT);

	QUADSPI_DLR = 255;
	QUADSPI_AR = addr;
	QUADSPI_CCR = ccr; /* go write a page */

  tmp = 0;
	do {
		sr = QUADSPI_SR;
		if (sr & QUADSPI_SR_TCF) {
			break;
		}
    tmp++;
    QUADSPI_BYTE_DR = *buf++;
	} while (QUADSPI_SR & QUADSPI_SR_BUSY);

  if (tmp != len) {
    usart_send_blocking(USART1, 0x1);
  }

	QUADSPI_FCR = 0x1f;
}

void blinkLed(){
  while(1){
    gpio_clear(GPIOB, GPIO0);
    gpio_toggle(GPIOC,GPIO10);
    for(int i=0; i<8000000; i++) {
      __asm__("nop");
    }
  }
}

int main(void) {
  init_clock();
  init_uart();
  init_led();

  //init_qspi();
  
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
  spi_init_master(
    SPI1, 
    SPI_CR1_BAUDRATE_FPCLK_DIV_2, 
    SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE, 
    SPI_CR1_CPHA_CLK_TRANSITION_1, 
    SPI_CR1_MSBFIRST
   ); 

  spi_enable_software_slave_management(SPI1);
  spi_set_nss_high(SPI1);
  //spi_set_unidirectional_mode(SPI1);
  spi_enable(SPI1);

  // check to see if peripheral is enabled
  while(!(SPI_SR(SPI1) & SPI_SR_TXE));

  gpio_clear(GPIOB, GPIO0);
  // wait for the peripheral to get ready
  //while(!(SPI_SR(SPI1) & SPI_SR_TXE));
  spi_send(SPI1, 0x0006); // enable write operation
  gpio_set(GPIOB, GPIO0);

  
  //write
  gpio_clear(GPIOB, GPIO0);
  spi_send(SPI1, 0xE1 << 4| 0x00);
  spi_send(SPI1, 0x00BB);
  gpio_set(GPIOB, GPIO0);


  // read
  gpio_clear(GPIOB, GPIO0);
  spi_send(SPI1, 0x03 << 4| 0x00);
  uint16_t data_flash = spi_read(SPI1);
  gpio_set(GPIOB, GPIO0);


  if (data_flash == 0xBB)
  {
    blinkLed();
  }
  
  
/*
  uint8_t data[256];
  for (int i = 0; i < 256; i++) {
    data[i] = (uint8_t) i;
  }

  qspi_write_page(0x8000000, data, 256);
  
  uint8_t st = read_register();
  //while(3){
  usart_send_blocking(USART1, st);
  //gpio_toggle(GPIOC,GPIO10);
  //}
  write_register((uint8_t) 1);
  read_register();
  //while(5){
  usart_send_blocking(USART1, st);
  //gpio_toggle(GPIOC,GPIO10);
 // }
*/

  return 0;
}
