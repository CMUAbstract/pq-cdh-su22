// flight.c
// Tests Tartan Artibeus CDH board flight bootloader
//
// Written by Bradley Denby
// Other contributors: None
//
// See the top-level LICENSE file for the license.

// Standard library
#include <stddef.h>          //
#include <stdint.h>          //
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
// pq-chd UART siupport library
#include </home/abstract/git-repos/pq-cdh-su22-master/software/uart-meeting/pq-cdh/support.h>      //

// Variables


// Main
int main(void) {
  // clock, uart, led initialization
  init_clock();
  init_uart();
  init_led();

/*  rx_cmd_buff_t rx_cmd_buff = {.size=CMD_MAX_LEN};
  clear_rx_cmd_buff(&rx_cmd_buff);
  tx_cmd_buff_t tx_cmd_buff = {.size=CMD_MAX_LEN};
  clear_tx_cmd_buff(&tx_cmd_buff);
  app_jump_pending = 0;

  // Bootloader loop
  while(1) {
    if(!app_jump_pending) {
      rx_usart1(&rx_cmd_buff);                 // Collect command bytes
      reply(&rx_cmd_buff, &tx_cmd_buff);       // Command reply logic
      tx_usart1(&tx_cmd_buff);                 // Send a response if any
    } else if(bl_check_app()) {                // Jump triggered; do basic check
      while(!tx_cmd_buff.empty) {              // If jumping to user app,
        tx_usart1(&tx_cmd_buff);               // finish sending response if any
      }
      for(size_t i=0; i<4000000; i++) {        // Wait for UART TX FIFO to flush
        __asm__ volatile("nop");
      }
      app_jump_pending = 0;                    // Housekeeping
      bl_jump_to_app();                        // Jump
    } else {                                   // If app_jump_pending &&
      app_jump_pending = 0;                    //  !bl_check_app()
    }                                          // Something wrong, abort jump
  }
*/


  // Loop
  while(1) {
    usart_send_blocking(USART1,'W');
    usart_send_blocking(USART1,'e');
    usart_send_blocking(USART1,'l');
    usart_send_blocking(USART1,'c');
    usart_send_blocking(USART1,'o');
    usart_send_blocking(USART1,'m');
    usart_send_blocking(USART1,'e');
    usart_send_blocking(USART1,' ');
    usart_send_blocking(USART1,'t');
    usart_send_blocking(USART1,'o');
    usart_send_blocking(USART1,' ');
    usart_send_blocking(USART1,'A');
    usart_send_blocking(USART1,'b');
    usart_send_blocking(USART1,'s');
    usart_send_blocking(USART1,'t');
    usart_send_blocking(USART1,'r');
    usart_send_blocking(USART1,'a');
    usart_send_blocking(USART1,'c');
    usart_send_blocking(USART1,'t');
    usart_send_blocking(USART1,' ');
    usart_send_blocking(USART1,'l');
    usart_send_blocking(USART1,'a');
    usart_send_blocking(USART1,'b');
    usart_send_blocking(USART1,'!');   
    usart_send_blocking(USART1,'\r');
    usart_send_blocking(USART1,'\n');
    gpio_toggle(GPIOC,GPIO10);
    for(int i=0; i<800000; i++) {
      __asm__("nop");
    }
  }

  // Should never reach this point
  return 0;
}
