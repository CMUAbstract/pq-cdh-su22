// cdh_flight_eps_com.c
// Makes the CDH board ctrl EPS and COM, PLD board

//
// See the top-level LICENSE file for the license.

// libopencm3
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include </home/abstract/git-repos/pq-cdh-su22-master/software/cdh_flight_eps_com/pq-cdh/support.h>
int main(void) {


//CDH - EPS control 

// initialization
  init_clock();
  init_led();
  init_uart();
  
  // enabling pheripherals to control EPS board
  
  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_GPIOD);
  rcc_periph_clock_enable(RCC_GPIOB);
  gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO15);
  gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO2);
  gpio_mode_setup(GPIOB, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO2);


  
 while(1){

  for(int i=0; i<32000000; i++) {
      __asm__("nop");

  gpio_clear(GPIOA, GPIO15);
  gpio_clear(GPIOD, GPIO2);
    }



  for(int i=0; i<32000000; i++) {
      __asm__("nop");

  gpio_set(GPIOA, GPIO15);
  gpio_set(GPIOD, GPIO2);
    }


}

}
