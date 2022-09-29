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
#include </home/abstract/git-repos/pq-cdh-su22-master/software/ext_osc_clk_mco_test/pq-cdh/support.h>
int main(void) {

  init_clock();

  //GPIO_InitTypeDef GPIO_InitStructure;

  //RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  rcc_periph_clock_enable(RCC_GPIOA);

  /* Output clock on MCO pin ---------------------------------------------*/
  gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO8);
  gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO8);
  gpio_set(GPIOA, GPIO8);
  //gpio_clear(GPIOC, GPIO12);
 /*
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
*/
  // pick one of the clocks to spew
  //RCC_MCOConfig(RCC_CFGR_MCO_HSE); // Put on MCO pin the: System clock selected
  //RCC_MCOConfig(RCC_MCOSource_HSE); // Put on MCO pin the: freq. of external crystal
  //RCC_MCOConfig(RCC_MCOSource_PLLCLK_Div2); // Put on MCO pin the: System clock selected
  //rcc_set_mco(RCC_CFGR_MCO_PLL); //for PLL clk test
  rcc_set_mco(RCC_CFGR_MCO_HSE);
}
