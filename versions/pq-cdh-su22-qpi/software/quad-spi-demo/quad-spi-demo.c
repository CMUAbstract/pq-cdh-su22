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
  init_qspi();
  // uint8_t buf[1] = {5};
  // qspi_writeData(buf, 1, 0x00000000);
  // uint8_t test = flash_readStatusRegister();

  // uint8_t mfrId, memory, capacity;
  // // flash_readProductInformation(&mfrId, &memory, &capacity);

  // // Create a dummy buffer
  // uint8_t dataBuffer_size = 10;
  // uint8_t dataBuffer[dataBuffer_size];
  // for(uint32_t i=0; i<dataBuffer_size; i++){
  //   dataBuffer[i] = i + 33;
  // }

  // uint32_t test_address = 0x00000010;

  // // Write the buffer to the flash
  // // flash_writeData(dataBuffer, dataBuffer_size, test_address);

  // // Read the buffer back from the flash
  // uint8_t dataBuffer_read[dataBuffer_size];
  // flash_readData(dataBuffer_read, dataBuffer_size, test_address);

  // flash_readProductInformation(&mfrId, &memory, &capacity);

  blinkLedBlocking();

  return 0;
}
