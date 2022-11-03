//#include <stdio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/dma.h>
#include </Users/saraltayal/Desktop/pocketqube/pq-cdh-su22/software/quad-spi-demo/pq-cdh/qspi.c>
#include <stdint.h>

int main(void)
{
  init_clock();
  // init_uart();
  init_led();
  // blinkLedBrief();
  blinkLedBrief();

  init_qspi();

  // qspi_waitForChip();
  // blinkLedBlocking();
  blinkLedBrief();

  uint8_t buf[1] = {5};
  qspi_writeData(buf, 1, 0x0);

  blinkLedBrief();
  qspi_waitForChip();

  // // // Read the buffer back from the flash
  uint8_t buf2[1] = {0};
  qspi_readDataWithAddress(buf2, 1, 0x0);

  blinkLedBlocking();

  return 0;
}
