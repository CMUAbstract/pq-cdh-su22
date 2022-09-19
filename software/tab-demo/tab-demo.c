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

// Macros
#define PAGE_BUFFER_LEN 128
#define ASCII_BUFFER_LEN 12

// Constants
static const uint16_t HW_ID = 0x0012;

// Variables
static uint16_t msg_id = 0x00;
static char cmd_buffer[CMD_MAX_LEN];
static char page_buffer[PAGE_BUFFER_LEN];
static char ascii_buffer[ASCII_BUFFER_LEN];

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

  ascii_buffer[0] = 'H';
  ascii_buffer[1] = 'e';
  ascii_buffer[2] = 'l';
  ascii_buffer[3] = 'l';
  ascii_buffer[4] = 'o';
  ascii_buffer[5] = ' ';
  ascii_buffer[6] = 'w';
  ascii_buffer[7] = 'o';
  ascii_buffer[8] = 'r';
  ascii_buffer[9] = 'l';
  ascii_buffer[10] = 'd';
  ascii_buffer[11] = '!';

  // Bootloader loop
  while(1) {
    gen_common_ascii(
     cmd_buffer,CMD_MAX_LEN,HW_ID,msg_id,LST,ascii_buffer,ASCII_BUFFER_LEN
    );
    msg_id = (msg_id+1)%65536;
    uart_puts(cmd_buffer,21);
    gpio_toggle(GPIOC,GPIO10);
    gpio_toggle(GPIOC,GPIO12);

    //Delay
    for(int i=0; i<8000000; i++) {
      __asm__("nop");
    }
  }

  // Should never reach this point
  return 0;
}
