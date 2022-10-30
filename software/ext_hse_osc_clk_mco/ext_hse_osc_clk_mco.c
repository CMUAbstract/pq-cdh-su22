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
#include </home/abstract/git-repos/pq-cdh-su22-master/software/ext_hse_osc_clk_mco/pq-cdh/support.h>
int main(void) {

  init_clock();

  
  rcc_periph_clock_enable(RCC_GPIOA);

  /* Output clock on MCO pin ---------------------------------------------*/
  gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO8);
  gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO8);
  gpio_set(GPIOA, GPIO8);
 
  rcc_set_mco(RCC_CFGR_MCO_PLL); //for PLL clk test : 100 MHz
  //rcc_set_mco(RCC_CFGR_MCO_HSE);  // for checking HSE output : 16 MHz

  // LED pin enable for debugging
  rcc_periph_clock_enable(RCC_GPIOC);
  gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO10);
  gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);
  gpio_set(GPIOC, GPIO10);
  gpio_clear(GPIOC, GPIO12);
  while(1) {
    for(int i=0; i<1600000; i++) {
      __asm__("nop");
    }
    gpio_toggle(GPIOC, GPIO10);
    gpio_toggle(GPIOC, GPIO12);
  }

}
