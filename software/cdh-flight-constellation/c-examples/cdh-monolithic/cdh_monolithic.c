// cdh_monolithic.c
// TAB C Example CDH Monolithic
//
// Written by Bradley Denby
// Other contributors: Alok Anand
//
// See the top-level LICENSE file for the license.

// Standard library headers
#include <stddef.h> // size_t
#include <stdint.h> // uint8_t, uint32_t

// Board-specific header
#include <cdh.h>    // CDH header
#include <libopencm3/stm32/rcc.h>

// TAB header
#include <tab.h>    // TAB header
#include <libopencm3/stm32/gpio.h>  // used in init_gpio

// Main
int main(void) {
  // MCU initialization
  init_clock();
  init_leds();
  init_uart();
  

  // TAB initialization
  rx_cmd_buff_t rx_cmd_buff = {.size=CMD_MAX_LEN};
  clear_rx_cmd_buff(&rx_cmd_buff);
  tx_cmd_buff_t tx_cmd_buff = {.size=CMD_MAX_LEN};
  clear_tx_cmd_buff(&tx_cmd_buff);
  uint8_t hw_matched = 0;
  
  // TAB loop
  while(1) {
  
    rx_usart1(&rx_cmd_buff);           // Collect command bytes
    hw_matched = rx_usart_hw_check(&rx_cmd_buff);  // check if configured HWIDs matches
    if (hw_matched == 1)                           //if HWIDs matched, parse TAB command and form response
    {
    hw_matched =0;
    reply(&rx_cmd_buff, &tx_cmd_buff); // Command reply logic
    tx_usart1(&tx_cmd_buff);           // Send a response if any
    }
    
   }
   
  // Should never reach this point
  return 0;
}
