// flight_401_blr.c
// Tartan Artibeus EXPT board flight 401 bootloader
//
// Written by Bradley Denby
// Other contributors: None
//
// See the top-level LICENSE file for the license.

// Standard library
#include <stddef.h>          // size_t
#include <stdint.h>          // fixed-width integer types

#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

// support library
#include <support.h>      // microcontroller utility functions
#include <commands.h>        // generates tab commands for transmission
#include <bootloader.h>      // microcontroller utility functions
#include <taolst_protocol.h> // protocol utility functions
 // protocol utility functions

// Macros
#define PAGE_BUFFER_LEN 128
#define ASCII_BUFFER_LEN 24  // changed from 12 for testing
//#define CMD_MAX_LEN 258

// Constants
static const uint16_t HW_ID = 0x0012;

// Variables
static uint16_t msg_id = 0x00;
static char cmd_buffer[CMD_MAX_LEN];
static char page_buffer[PAGE_BUFFER_LEN];
static char ascii_buffer[ASCII_BUFFER_LEN];


//static char rece_buffer [CMD_MAX_LEN];

//// in_bootloader is an extern variable read by bootloader_running
int in_bootloader;

//// app_jump_pending is an extern variable modified in write_reply
int app_jump_pending;

//Helper Functions
static void uart_puts(const char* s, const size_t l) {
  for(size_t i=0; i<l; i++) {
    usart_send_blocking(USART1,s[i]);
  }
}

// Main
int main(void) {
  
  // Bootloader initialization
  init_clock();
  init_led();
  init_uart();


  // enabling pheripherals to control EPS board
  // IO pin ctrl to EPS
  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_GPIOD);
  rcc_periph_clock_enable(RCC_GPIOB);
  gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO15);
  gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO2);
  gpio_mode_setup(GPIOB, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO2);
  gpio_set(GPIOA, GPIO15);
  gpio_set(GPIOD, GPIO2);

  //LED pin
  rcc_periph_clock_enable(RCC_GPIOC);
  gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO10);
  gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);

  
  rx_cmd_buff_t rx_cmd_buff = {.size=CMD_MAX_LEN};
  clear_rx_cmd_buff(&rx_cmd_buff);
  tx_cmd_buff_t tx_cmd_buff = {.size=CMD_MAX_LEN};
  clear_tx_cmd_buff(&tx_cmd_buff);

  // Application loop
  while(1) {
  
    
    rx_usart1(&rx_cmd_buff);           // Collect command bytes
	  gpio_toggle(GPIOC, GPIO10);
	
    reply(&rx_cmd_buff, &tx_cmd_buff); // Command reply logic
	
    tx_usart1(&tx_cmd_buff);           // Send a response if any
	  gpio_toggle(GPIOC, GPIO12);
      
    
    //usart_recv_blocking(USART1);
    //usart_send(USART1, usart_recv(USART1));
    
    
  }

	return 0;
}

