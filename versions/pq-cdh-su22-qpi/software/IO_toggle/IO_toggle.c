// blink.c
// Makes the CDH board LEDs blink
//
// Written by Bradley Denby
// Other contributors: None
//
// See the top-level LICENSE file for the license.

// libopencm3
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

int main(void) {
  //rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_GPIOD);
  //gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO15);
  gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO2);
  //gpio_set(GPIOA, GPIO15);
  gpio_clear(GPIOD, GPIO2);
  while(1) {
    for(int i=0; i<10000000; i++) {
      __asm__("nop");
    }
    //gpio_toggle(GPIOA, GPIO15);
    gpio_toggle(GPIOD, GPIO2);
  }
}
