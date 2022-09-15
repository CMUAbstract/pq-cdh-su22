// support.h
// Tartan Artibeus CDH board offchip-flash support file
//
// Written by Bradley Denby
// Other contributors: None
//
// See the top-level LICENSE file for the license.

#ifndef STM32L4
#define STM32L4
#endif

#ifndef SUPPORT_H
#define SUPPORT_H
// Initialization functions

void init_clock(void);
void init_led(void);
void init_qspi(void);
void init_uart(void);

#endif
