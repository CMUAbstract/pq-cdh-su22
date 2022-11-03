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

  //GPIO_InitTypeDef GPIO_InitStructure;

  //RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  rcc_periph_clock_enable(RCC_GPIOA);

  /* Output clock on MCO pin ---------------------------------------------*/
  gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO8);
  gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO8);
  gpio_set(GPIOA, GPIO8);
  
  
  /*
  GPIO_MODE_AF : GPIO alternate function ( Analog/Digital)
 gpio_set_output_options 
 # Parameters
[in]	gpioport	Unsigned int32. Port identifier GPIO Port IDs
[in]	otype	Unsigned int8. Pin output type GPIO Output Pin Driver Type
  GPIO_OTYPE_PP, GPIO_OTYPE_D   PushPull. OpenDrain
[in]	speed	Unsigned int8. Pin speed GPIO Output Pin Speed
   #define 	GPIO_OSPEED_LOW   0x0 
  #define 	GPIO_OSPEED_MED   0x1
  #define 	GPIO_OSPEED_HIGH   0x2
  #define 	GPIO_OSPEED_VERYHIGH   0x3
  #define 	GPIO_OSPEED_2MHZ   0x0
  #define 	GPIO_OSPEED_25MHZ   0x1
  #define 	GPIO_OSPEED_50MHZ   0x2
  #define 	GPIO_OSPEED_100MHZ   0x3
[in]	gpios	Unsigned int16. Pin identifiers GPIO Pin Identifiers 
If multiple pins are to be set, use bitwise OR '|' to separate them
  
  
  */
 
  //RCC_MCOConfig(RCC_MCOSource_PLLCLK_Div2); // Put on MCO pin the: System clock selected
  //rcc_set_mco(RCC_CFGR_MCO_PLL); //for PLL clk test
  rcc_set_mco(RCC_CFGR_MCO_HSE);

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
