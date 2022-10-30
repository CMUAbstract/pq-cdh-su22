// support.c
// Tartan Artibeus CDH board UART support implementation file
//
// Written by Bradley Denby
// Other contributors: None
//
// See the top-level LICENSE file for the license.

// Standard library
#include <stdint.h>                 // uint8_t

// libopencm3 library
#include <libopencm3/cm3/scb.h>     // SCB_VTOR
#include <libopencm3/stm32/flash.h> // used in init_clock
#include <libopencm3/stm32/gpio.h>  // used in init_gpio
#include <libopencm3/stm32/rcc.h>   // used in init_clock
#include <libopencm3/stm32/usart.h> // used in init_uart

// UART support library
#include </home/abstract/git-repos/pq-cdh-su22-master/software/int_pll_clk_mco_test/pq-cdh/support.h>             // Header file

// Initialization functions

void init_clock(void) {

  rcc_osc_on(RCC_HSI16);                      // 80 MHz PLL
  rcc_wait_for_osc_ready(RCC_HSI16);          // Wait until PLL is ready  

  rcc_set_hpre(RCC_CFGR_HPRE_NODIV);        // AHB at 80 MHz (80 MHz max.)
  rcc_set_ppre1(RCC_CFGR_PPRE_DIV2);       // APB1 at 40 MHz (80 MHz max.)
  rcc_set_ppre2(RCC_CFGR_PPRE_NODIV);      // APB2 at 80 MHz (80 MHz max.)

  rcc_osc_off(RCC_PLL);

  rcc_set_main_pll(                         // Setup 80 MHz clock
   RCC_PLLCFGR_PLLSRC_HSI16,                // PLL clock source
   16,                                       // PLL VCO division factor
   40,                                      // PLL VCO multiplication factor
   0,                                       // PLL P clk output division factor
   0,                                       // PLL Q clk output division factor
   RCC_PLLCFGR_PLLR_DIV4                    // PLL sysclk output division factor
   //RCC_PLLCFGR_PLLR_DIV8  
  );
  // 16MHz/4 = 4MHz; 4MHz*40 = 160MHz VCO; 160MHz/2 = 80MHz PLL
  // 16MHz/4 = 4MHz; 4MHz*16 = 64MHz VCO; 64MHz/2 = 32MHz PLL
  // 16MHz/4 = 4MHz; 4MHz*8 = 32MHz VCO; 32MHz/2 = 16MHz PLL
  // 16MHz/4 = 4MHz; 4MHz*4 = 16MHz VCO; 16MHz/2 = 8MHz PLL
  // 16MHz/4 = 4MHz; 4MHz*1 = 4MHz VCO; 2MHz/2 = 2MHz PLL
  
  //Division factor
  //Path : libopencm3/include/libopencm3/stm32/l4
  /*
   RCC_PLLCFGR_PLLR_DIV2		0   =>8MHz
   RCC_PLLCFGR_PLLR_DIV4		1   =>4MHz
   RCC_PLLCFGR_PLLR_DIV6		2  => 2MHz
   RCC_PLLCFGR_PLLR_DIV8		3
  */
  rcc_osc_on(RCC_PLL);                      // 80 MHz PLL
  rcc_wait_for_osc_ready(RCC_PLL);          // Wait until PLL is ready

  rcc_set_sysclk_source(RCC_CFGR_SW_PLL); 
  rcc_wait_for_sysclk_status(RCC_PLL);  

  rcc_ahb_frequency = 80000000;//160000000;//80000000;
  rcc_apb1_frequency = 40000000;
  rcc_apb2_frequency = 80000000;
}

void init_led(void) {
  rcc_periph_clock_enable(RCC_GPIOC);
  gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO10);
  gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);
  gpio_set(GPIOC, GPIO10);
  gpio_clear(GPIOC, GPIO12);
}

void init_uart(void) {
  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_USART1);
  gpio_mode_setup(GPIOA,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO9|GPIO10);
  gpio_set_af(GPIOA,GPIO_AF7,GPIO9);  // USART1_TX is alternate function 7
  gpio_set_af(GPIOA,GPIO_AF7,GPIO10); // USART1_RX is alternate function 7
  usart_set_baudrate(USART1,115200);
  usart_set_databits(USART1,8);
  usart_set_stopbits(USART1,USART_STOPBITS_1);
  usart_set_mode(USART1,USART_MODE_TX_RX);
  usart_set_parity(USART1,USART_PARITY_NONE);
  usart_set_flow_control(USART1,USART_FLOWCONTROL_NONE);
  usart_enable(USART1);
}


/*
void init_clock(void) {
  rcc_set_sysclk_source(RCC_CFGR_SW_PLL); // Sets sysclk source for RTOS
  rcc_set_hpre(RCC_CFGR_HPRE_NODIV);        // AHB at 80 MHz (80 MHz max.)
  rcc_set_ppre1(RCC_CFGR_PPRE_DIV2);       // APB1 at 40 MHz (80 MHz max.)
  rcc_set_ppre2(RCC_CFGR_PPRE_NODIV);      // APB2 at 80 MHz (80 MHz max.)
  rcc_set_main_pll(                         // Setup 80 MHz clock
   RCC_PLLCFGR_PLLSRC_HSI16,                // PLL clock source
   40,                                       // PLL VCO division factor
   4,                                      // PLL VCO multiplication factor
   0,                                       // PLL P clk output division factor
   0,                                       // PLL Q clk output division factor
   RCC_PLLCFGR_PLLR_DIV2                    // PLL sysclk output division factor
  );
  // 16MHz/4 = 4MHz; 4MHz*40 = 160MHz VCO; 160MHz/2 = 80MHz PLL
  // 16MHz/4 = 4MHz; 4MHz*16 = 64MHz VCO; 64MHz/2 = 32MHz PLL
  // 16MHz/4 = 4MHz; 4MHz*8 = 32MHz VCO; 32MHz/2 = 16MHz PLL
  // 16MHz/4 = 4MHz; 4MHz*4 = 16MHz VCO; 16MHz/2 = 8MHz PLL
  // 16MHz/4 = 4MHz; 4MHz*1 = 4MHz VCO; 2MHz/2 = 2MHz PLL
  
  rcc_osc_on(RCC_HSI16);                      // 80 MHz PLL
  rcc_wait_for_osc_ready(RCC_HSI16);          // Wait until PLL is ready  

  rcc_osc_on(RCC_PLL);                      // 80 MHz PLL
  rcc_wait_for_osc_ready(RCC_PLL);          // Wait until PLL is ready
  
  rcc_ahb_frequency = 80000000;//160000000;//80000000;
  rcc_apb1_frequency = 40000000;
  rcc_apb2_frequency = 80000000;
}*/
