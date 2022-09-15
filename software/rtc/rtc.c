// bootloader.c
// Tartan Artibeus CDH board rtc implementation file
// rtc support library
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
#include <libopencm3/stm32/rtc.h>
#include <libopencm3/stm32/pwr.h>


// rtc support library
#include </home/abstract/git-repos/pq-cdh-su22-master/software/rtc/pq-cdh/support.h>             // Header file

// Initialization functions
void set_rtc(void);

void set_rtc(void) {
  rtc_enable_bypass_shadow_register();
  rtc_calendar_set_year((uint8_t) 11);
  //RTC_DR = 0x00113109;
  usart_send_blocking(USART1, RTC_DR >> 16);
  
  //set RTC_DR_MT,      Date Register bit  [12] 
  //and RTC_DR_MU[3:0], Date Register bits [11:8]
  rtc_calendar_set_month((uint8_t) 11);

  //set RTC_DR_DT[1:0], Date Register bits [5:4] 
  //and RTC_DR_DU[3:0], Date Register bits [3:0]
  rtc_calendar_set_day((uint8_t) 12);
  usart_send_blocking(USART1, RTC_DR & 0xff);

  rtc_time_set_time((uint8_t) 10, (uint8_t) 0, (uint8_t) 0, false);
  rtc_set_am_format();

  rtc_clear_init_flag();
  rtc_lock();
  pwr_enable_backup_domain_write_protect();
}

int app_subroutine(int32_t seconds, int32_t nanoseconds);

int app_subroutine(int32_t seconds, int32_t nanoseconds) {
  int32_t JD = 2451545 + (43200 + seconds) / 86400;
  // int32_t JD = 2451545 + (seconds + 21600) / 86400;
  int32_t I, J, K, L, N;
  L = JD + 68569;
  N = 4 * L / 146097;
  L = L - (146097 * N + 3) / 4;
  I = 4000 * (L + 1) / 1461001;
  L = L - 1461 * I / 4 + 31;
  J = 80 * L / 2447;
  K = L - 2447 * J / 80;
  L = J / 11;
  J = J + 2 - 12 * L;
  I = 100 * (N - 49) + I + L;

  int32_t newJD = K - 32075 + 1461*(I + 4800 + (J - 14)/12)/4
                   + 367*(J - 2 - (J - 14)/12*12)/12 - 3
                   *((I + 4900 + (J - 14)/12)/100)/4;
  int32_t remainder = 43200 + seconds - (newJD - 2451545) * 86400; //seconds
  int32_t hour = remainder / 3600;
  int32_t min = (remainder % 3600) / 60;
  int32_t sec = (remainder % 3600) % 60;

  usart_send_blocking(USART1, (uint8_t) (I - 2000));
  usart_send_blocking(USART1, (uint8_t) J);
  usart_send_blocking(USART1, (uint8_t) K);
  /*
  usart_send_blocking(USART1, (uint8_t) ((JD >> 24) & 0xff));
  usart_send_blocking(USART1, (uint8_t) ((JD >> 16) & 0xff));
  usart_send_blocking(USART1, (uint8_t) ((JD >> 8) & 0xff));
  usart_send_blocking(USART1, (uint8_t) (JD & 0xff));
  usart_send_blocking(USART1, (uint8_t) ((newJD >> 24) & 0xff));
  usart_send_blocking(USART1, (uint8_t) ((newJD >> 16) & 0xff));
  usart_send_blocking(USART1, (uint8_t) ((newJD >> 8) & 0xff));
  usart_send_blocking(USART1, (uint8_t) (newJD & 0xff));*/
  usart_send_blocking(USART1, (uint8_t) hour);
  usart_send_blocking(USART1, (uint8_t) min);
  usart_send_blocking(USART1, (uint8_t) sec);

  
  //procedure to initialize rtc
  rcc_periph_clock_enable(RCC_PWR);
  pwr_disable_backup_domain_write_protect();
  rcc_osc_on(RCC_LSI);
  rcc_wait_for_osc_ready(RCC_LSI);
  rcc_set_rtc_clock_source(RCC_LSI);
  rcc_enable_rtc_clock();
  rtc_wait_for_synchro();
  rtc_unlock();
  rtc_set_init_flag();
  rtc_wait_for_init_ready();
  rtc_set_prescaler((uint32_t) 249, (uint32_t) 127);
  rtc_enable_bypass_shadow_register();

  //set RTC_DR_YT[3:0], Date Register bits [23:20] 
  //and RTC_DR_YU[3:0], Date Register bits [19:16]
  rtc_calendar_set_year((uint8_t) (I - 2000));
  
  //set RTC_DR_MT,      Date Register bit  [12] 
  //and RTC_DR_MU[3:0], Date Register bits [11:8]
  rtc_calendar_set_month((uint8_t) J);

  //set RTC_DR_DT[1:0], Date Register bits [5:4] 
  //and RTC_DR_DU[3:0], Date Register bits [3:0]
  rtc_calendar_set_day((uint8_t) K);
  
  rtc_set_am_format();
  //set RTC_TR_PM,       Time Register bit  [22]
  //and RTC_TR_HT[3:0],  Time Register bits [21:20]
  //and RTC_TR_HU[3:0],  Time Register bits [19:16]
  //and RTC_TR_MNT[3:0], Time Register bits [14:12]
  //and RTC_TR_MNU[3:0], Time Register bits [11:8]
  //and RTC_TR_ST[3:0],  Time Register bits [6:4]
  //and RTC_TR_SU[3:0],  Time Register bits [3:0]
  rtc_time_set_time((uint8_t) hour, (uint8_t) min, (uint8_t) sec, true);

  rtc_clear_init_flag();
  rtc_lock();
  pwr_enable_backup_domain_write_protect();
  
  //trivial return statement, currently has no way to set nanoseconds
  return nanoseconds * 0 + 1;
}

uint32_t app_read_time(void);

uint32_t app_read_time() {
  int32_t I = (int32_t) (((RTC_DR >> 20) * 10) + ((RTC_DR >> 16) & 0xf) + 2000); //year
  int32_t J = (int32_t) ((((RTC_DR >> 12) & 0x1) * 10) + ((RTC_DR >> 8) & 0xf)); //month
  int32_t K = (int32_t) ((((RTC_DR >> 4) & 0x3) * 10) + (RTC_DR & 0xf)); //day
  int32_t hour = (int32_t) ((((RTC_TR >> 20) & 0x3) * 10) + ((RTC_TR >> 16) & 0xf));
  int32_t minute = (int32_t) ((((RTC_TR >> 12) & 0x7) * 10) + ((RTC_TR >> 8) & 0xf));
  int32_t second = (int32_t) ((((RTC_TR >> 4) & 0x7) * 10) + (RTC_TR & 0xf));
  
  int32_t JD = K - 32075 + 1461*(I + 4800 + (J - 14)/12)/4
               + 367*(J - 2 - (J - 14)/12*12)/12 - 3
               *((I + 4900 + (J - 14)/12)/100)/4;
  int32_t JS = (JD - 2451545) * 86400 - 43200;
  int32_t additional = hour * 3600 + minute * 60 + second;
  uint32_t res = (uint32_t) (JS + additional);
  return res;
}

int main(void) {
  // Bootloader initialization
  init_clock();
  init_uart();
  init_led();
  app_subroutine(679674848, 640221000);
  while(1) {
    /*
    usart_send_blocking(USART1,'H');
    usart_send_blocking(USART1,'e');
    usart_send_blocking(USART1,'l');
    usart_send_blocking(USART1,'l');
    usart_send_blocking(USART1,'o');
    usart_send_blocking(USART1,' ');
    usart_send_blocking(USART1,'w');
    usart_send_blocking(USART1,'o');
    usart_send_blocking(USART1,'r');
    usart_send_blocking(USART1,'l');
    usart_send_blocking(USART1,'d');
    usart_send_blocking(USART1,'!');
    usart_send_blocking(USART1,'\r');
    usart_send_blocking(USART1,'\n');
    */
    gpio_toggle(GPIOC,GPIO10);
    for(int i=0; i<800000; i++) {
      __asm__("nop");
    }
  }
}
