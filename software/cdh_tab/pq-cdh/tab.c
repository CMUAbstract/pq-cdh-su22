// tab.c
// Tartan Artibeus BUS serial communication protocol implementation file
//
// Written by Bradley Denby
// Other contributors: Shize Che, Chad Taylor
//
// See the top-level LICENSE file for the license.

// Standard library
#include <stddef.h>                 // size_t
#include <stdint.h>                 // uint8_t, uint32_t, uint64_t
#include <stdio.h>                  // snprintf

#include <tab.h>        // Header file

// User-file for mcu specific functions and extern variables
#include <mcu_tab.h>

// Variables
extern int in_bootloader;    // Used in bootloader main to indicate MCU state
extern int app_jump_pending; // Used in bootloader main to signal jump to app

// Helper functions

//// Clears rx_cmd_buff data and resets state and indices
void clear_rx_cmd_buff(rx_cmd_buff_t* rx_cmd_buff_o) {
  rx_cmd_buff_o->state = RX_CMD_BUFF_STATE_START_BYTE_0;
  rx_cmd_buff_o->start_index = 0;
  rx_cmd_buff_o->end_index = 0;
  for(size_t i=0; i<rx_cmd_buff_o->size; i++) {
    rx_cmd_buff_o->data[i] = ((uint8_t)0x00);
  }
}

//// Clears tx_cmd_buff data and resets state and indices
void clear_tx_cmd_buff(tx_cmd_buff_t* tx_cmd_buff_o) {
  tx_cmd_buff_o->empty = 1;
  tx_cmd_buff_o->start_index = 0;
  tx_cmd_buff_o->end_index = 0;
  for(size_t i=0; i<tx_cmd_buff_o->size; i++) {
    tx_cmd_buff_o->data[i] = ((uint8_t)0x00);
  }
}

//// Indicates whether MCU is in bootloader mode or application mode
int bootloader_running(void) {
  return in_bootloader;
}

// Command functions

//// BOOTLOADER_ERASE
int bootloader_erase(void) {
  return mcu_bootloader_erase();
}

//// Given a well-formed BOOTLOADER_WRITE_PAGE command, write data to flash
int bootloader_write_data(rx_cmd_buff_t* rx_cmd_buff) {
  return mcu_bootloader_write_data(rx_cmd_buff);
}

// Protocol functions

//// Attempts to push byte to end of rx_cmd_buff
void push_rx_cmd_buff(rx_cmd_buff_t* rx_cmd_buff_o, uint8_t b) {
  switch(rx_cmd_buff_o->state) {
    case RX_CMD_BUFF_STATE_START_BYTE_0:
      if(b==START_BYTE_0) {
        rx_cmd_buff_o->data[START_BYTE_0_INDEX] = b;
        rx_cmd_buff_o->state = RX_CMD_BUFF_STATE_START_BYTE_1;
      }
      break;
    case RX_CMD_BUFF_STATE_START_BYTE_1:
      if(b==START_BYTE_1) {
        rx_cmd_buff_o->data[START_BYTE_1_INDEX] = b;
        rx_cmd_buff_o->state = RX_CMD_BUFF_STATE_MSG_LEN;
      } else {
        clear_rx_cmd_buff(rx_cmd_buff_o);
      }
      break;
    case RX_CMD_BUFF_STATE_MSG_LEN:
      if(((uint8_t)0x06)<=b /*&& b<=((uint8_t)0xff)*/) {
        rx_cmd_buff_o->data[MSG_LEN_INDEX] = b;
        rx_cmd_buff_o->start_index = ((uint8_t)0x09);
        rx_cmd_buff_o->end_index = (b+((uint8_t)0x03));
        rx_cmd_buff_o->state = RX_CMD_BUFF_STATE_HWID_LSB;
      } else {
        clear_rx_cmd_buff(rx_cmd_buff_o);
      }
      break;
    case RX_CMD_BUFF_STATE_HWID_LSB:
      rx_cmd_buff_o->data[HWID_LSB_INDEX] = b;
      rx_cmd_buff_o->state = RX_CMD_BUFF_STATE_HWID_MSB;
      break;
    case RX_CMD_BUFF_STATE_HWID_MSB:
      rx_cmd_buff_o->data[HWID_MSB_INDEX] = b;
      rx_cmd_buff_o->state = RX_CMD_BUFF_STATE_MSG_ID_LSB;
      break;
    case RX_CMD_BUFF_STATE_MSG_ID_LSB:
      rx_cmd_buff_o->data[MSG_ID_LSB_INDEX] = b;
      rx_cmd_buff_o->state = RX_CMD_BUFF_STATE_MSG_ID_MSB;
      break;
    case RX_CMD_BUFF_STATE_MSG_ID_MSB:
      rx_cmd_buff_o->data[MSG_ID_MSB_INDEX] = b;
      rx_cmd_buff_o->state = RX_CMD_BUFF_STATE_DEST_ID;
      break;
    case RX_CMD_BUFF_STATE_DEST_ID:
      rx_cmd_buff_o->data[DEST_ID_INDEX] = b;
      rx_cmd_buff_o->state = RX_CMD_BUFF_STATE_OPCODE;
      break;
    case RX_CMD_BUFF_STATE_OPCODE: // no check for valid opcodes (too much)
      rx_cmd_buff_o->data[OPCODE_INDEX] = b;
      if(rx_cmd_buff_o->start_index<rx_cmd_buff_o->end_index) {
        rx_cmd_buff_o->state = RX_CMD_BUFF_STATE_DATA;
      } else {
        rx_cmd_buff_o->state = RX_CMD_BUFF_STATE_COMPLETE;
      }
      break;
    case RX_CMD_BUFF_STATE_DATA:
      if(rx_cmd_buff_o->start_index<rx_cmd_buff_o->end_index) {
        rx_cmd_buff_o->data[rx_cmd_buff_o->start_index] = b;
        rx_cmd_buff_o->start_index += 1;
      }
      // Must move to COMPLETE state immediately if b is the last byte
      if(rx_cmd_buff_o->start_index==rx_cmd_buff_o->end_index) {
        rx_cmd_buff_o->state = RX_CMD_BUFF_STATE_COMPLETE;
      }
      break;
    case RX_CMD_BUFF_STATE_COMPLETE:
      // If rx_cmd_buff_t holds a complete command, do nothing with new byte b
      break;
    default:
      // Do nothing by default
      break;
  }
}

//// Attempts to clear rx_cmd_buff and populate tx_cmd_buff with reply
void write_reply(rx_cmd_buff_t* rx_cmd_buff_o, tx_cmd_buff_t* tx_cmd_buff_o) {
  if(
   rx_cmd_buff_o->state==RX_CMD_BUFF_STATE_COMPLETE &&
   tx_cmd_buff_o->empty
  ) {
    tx_cmd_buff_o->data[START_BYTE_0_INDEX] = START_BYTE_0;
    tx_cmd_buff_o->data[START_BYTE_1_INDEX] = START_BYTE_1;
    tx_cmd_buff_o->data[HWID_LSB_INDEX] = rx_cmd_buff_o->data[HWID_LSB_INDEX];
    tx_cmd_buff_o->data[HWID_MSB_INDEX] = rx_cmd_buff_o->data[HWID_MSB_INDEX];
    tx_cmd_buff_o->data[MSG_ID_LSB_INDEX] =
     rx_cmd_buff_o->data[MSG_ID_LSB_INDEX];
    tx_cmd_buff_o->data[MSG_ID_MSB_INDEX] =
     rx_cmd_buff_o->data[MSG_ID_MSB_INDEX];
    tx_cmd_buff_o->data[DEST_ID_INDEX] =
     (0x0f & rx_cmd_buff_o->data[DEST_ID_INDEX]) << 4 |
     (0xf0 & rx_cmd_buff_o->data[DEST_ID_INDEX]) >> 4;
    // useful variables
    size_t i     = 0;
    uint32_t sec = 0;
    uint32_t ns  = 0;
    int success  = 0;
    switch(rx_cmd_buff_o->data[OPCODE_INDEX]) {
      case APP_GET_TELEM_OPCODE:
        tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x54);
        tx_cmd_buff_o->data[OPCODE_INDEX] = APP_TELEM_OPCODE;
        for(i=DATA_START_INDEX; i<((size_t)0x4e); i++) {
          tx_cmd_buff_o->data[i] = ((uint8_t)0x00);
        }
        break;
      case APP_GET_TIME_OPCODE:
        // initialize common variables to known values
        sec     = 0;
        ns      = 0;
        success = 0;
        success = get_rtc(&sec, &ns);
        if(success) {
          tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x0e);
          tx_cmd_buff_o->data[OPCODE_INDEX] = APP_SET_TIME_OPCODE;
          uint8_t sec_0 = (sec >>  0) & 0xff; // LSB
          uint8_t sec_1 = (sec >>  8) & 0xff;
          uint8_t sec_2 = (sec >> 16) & 0xff;
          uint8_t sec_3 = (sec >> 24) & 0xff; // MSB
          uint8_t ns_0  = (ns  >>  0) & 0xff; // LSB
          uint8_t ns_1  = (ns  >>  8) & 0xff;
          uint8_t ns_2  = (ns  >> 16) & 0xff;
          uint8_t ns_3  = (ns  >> 24) & 0xff; // MSB
          tx_cmd_buff_o->data[DATA_START_INDEX+0] = sec_0;
          tx_cmd_buff_o->data[DATA_START_INDEX+1] = sec_1;
          tx_cmd_buff_o->data[DATA_START_INDEX+2] = sec_2;
          tx_cmd_buff_o->data[DATA_START_INDEX+3] = sec_3;
          tx_cmd_buff_o->data[DATA_START_INDEX+4] =  ns_0;
          tx_cmd_buff_o->data[DATA_START_INDEX+5] =  ns_1;
          tx_cmd_buff_o->data[DATA_START_INDEX+6] =  ns_2;
          tx_cmd_buff_o->data[DATA_START_INDEX+7] =  ns_3;
        } else {
          tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
          tx_cmd_buff_o->data[OPCODE_INDEX] = COMMON_NACK_OPCODE;
        }
        break;
      case APP_REBOOT_OPCODE:
        tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
        tx_cmd_buff_o->data[OPCODE_INDEX] = COMMON_NACK_OPCODE;
        break;
      case APP_SET_TIME_OPCODE:
        ; // empty statement to avoid weird C thing about vars in switch case
        // collect bytes
        uint8_t sec_0 = rx_cmd_buff_o->data[DATA_START_INDEX+0]; // LSB
        uint8_t sec_1 = rx_cmd_buff_o->data[DATA_START_INDEX+1];
        uint8_t sec_2 = rx_cmd_buff_o->data[DATA_START_INDEX+2];
        uint8_t sec_3 = rx_cmd_buff_o->data[DATA_START_INDEX+3]; // MSB
        uint8_t ns_0  = rx_cmd_buff_o->data[DATA_START_INDEX+4]; // LSB
        uint8_t ns_1  = rx_cmd_buff_o->data[DATA_START_INDEX+5];
        uint8_t ns_2  = rx_cmd_buff_o->data[DATA_START_INDEX+6];
        uint8_t ns_3  = rx_cmd_buff_o->data[DATA_START_INDEX+7]; // MSB
        // initialize common variables to known values
        sec     = 0;
        ns      = 0;
        success = 0;
        // assemble bytes
        sec = (uint32_t)((sec_3<<24) | (sec_2<<16) | (sec_1<<8) | (sec_0<<0));
        ns  = (uint32_t)(( ns_3<<24) | ( ns_2<<16) | ( ns_1<<8) | ( ns_0<<0));
        // use assembled bytes
        success = set_rtc(sec, ns);
        // reply
        if(success) {
          tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
          tx_cmd_buff_o->data[OPCODE_INDEX] = COMMON_ACK_OPCODE;
        } else {
          tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
          tx_cmd_buff_o->data[OPCODE_INDEX] = COMMON_NACK_OPCODE;
        }
        break;
      case APP_TELEM_OPCODE:
        tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
        tx_cmd_buff_o->data[OPCODE_INDEX] = COMMON_NACK_OPCODE;
        break;
      case BOOTLOADER_ACK_OPCODE:
        tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
        tx_cmd_buff_o->data[OPCODE_INDEX] = COMMON_NACK_OPCODE;
        break;
      case BOOTLOADER_ERASE_OPCODE:
        if(bootloader_running()) {
          success = bootloader_erase();
          tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x07);
          tx_cmd_buff_o->data[OPCODE_INDEX] = BOOTLOADER_ACK_OPCODE;
          tx_cmd_buff_o->data[DATA_START_INDEX] = BOOTLOADER_ACK_REASON_ERASED;
        } else {
          tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
          tx_cmd_buff_o->data[OPCODE_INDEX] = COMMON_NACK_OPCODE;
        }
        break;
      case BOOTLOADER_NACK_OPCODE:
        tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
        tx_cmd_buff_o->data[OPCODE_INDEX] = COMMON_NACK_OPCODE;
        break;
      case BOOTLOADER_PING_OPCODE:
        if(bootloader_running()) {
          tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x07);
          tx_cmd_buff_o->data[OPCODE_INDEX] = BOOTLOADER_ACK_OPCODE;
          tx_cmd_buff_o->data[DATA_START_INDEX] = BOOTLOADER_ACK_REASON_PONG;
        } else {
          tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
          tx_cmd_buff_o->data[OPCODE_INDEX] = COMMON_NACK_OPCODE;
        }
        break;
      case BOOTLOADER_WRITE_PAGE_OPCODE:
        if(bootloader_running()) {
          // initialize common variables to known values
          success = 0;
          success = bootloader_write_data(rx_cmd_buff_o);
          if(success) {
            tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x07);
            tx_cmd_buff_o->data[OPCODE_INDEX] = BOOTLOADER_ACK_OPCODE;
            tx_cmd_buff_o->data[DATA_START_INDEX] =
             rx_cmd_buff_o->data[DATA_START_INDEX];
          } else {
            tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
            tx_cmd_buff_o->data[OPCODE_INDEX] = BOOTLOADER_NACK_OPCODE;
          }
        } else{
          tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
          tx_cmd_buff_o->data[OPCODE_INDEX] = COMMON_NACK_OPCODE;
        }
        break;
      case BOOTLOADER_WRITE_PAGE_ADDR32_OPCODE:
        // initialize common variables to known values
        success = 0;
        success = bootloader_write_data(rx_cmd_buff_o);
        if (bootloader_running()) {
          if(success) {
            tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x0A);
            tx_cmd_buff_o->data[OPCODE_INDEX] = BOOTLOADER_ACK_OPCODE;
            tx_cmd_buff_o->data[DATA_START_INDEX] =
            rx_cmd_buff_o->data[DATA_START_INDEX];
            tx_cmd_buff_o->data[DATA_START_INDEX+1] =
            rx_cmd_buff_o->data[DATA_START_INDEX+1];
            tx_cmd_buff_o->data[DATA_START_INDEX+2] =
            rx_cmd_buff_o->data[DATA_START_INDEX+2];
            tx_cmd_buff_o->data[DATA_START_INDEX+3] =
            rx_cmd_buff_o->data[DATA_START_INDEX+3];
          } else {
            tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
            tx_cmd_buff_o->data[OPCODE_INDEX] = BOOTLOADER_NACK_OPCODE;
          }
        } else{
          tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
          tx_cmd_buff_o->data[OPCODE_INDEX] = COMMON_NACK_OPCODE;
        }
        break;
      case BOOTLOADER_JUMP_OPCODE:
        if(bootloader_running()) {
          app_jump_pending = 1;
          tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x07);
          tx_cmd_buff_o->data[OPCODE_INDEX] = BOOTLOADER_ACK_OPCODE;
          tx_cmd_buff_o->data[DATA_START_INDEX] = BOOTLOADER_ACK_REASON_JUMP;
        } else {
          tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
          tx_cmd_buff_o->data[OPCODE_INDEX] = COMMON_NACK_OPCODE;
        }
        break;
      case COMMON_ACK_OPCODE:
        tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
        tx_cmd_buff_o->data[OPCODE_INDEX] = COMMON_ACK_OPCODE;
        break;
      case COMMON_DEBUG_OPCODE:
        tx_cmd_buff_o->data[MSG_LEN_INDEX] = rx_cmd_buff_o->data[MSG_LEN_INDEX];
        tx_cmd_buff_o->data[OPCODE_INDEX] = COMMON_DEBUG_OPCODE;
        uint32_t mess_len = rx_cmd_buff_o->data[MSG_LEN_INDEX] - 6;
        for (i = 0; i < mess_len; i++) {
          tx_cmd_buff_o->data[DATA_START_INDEX+i] = rx_cmd_buff_o->data[DATA_START_INDEX+i];
        }
        break;
      case COMMON_DATA_OPCODE:
        tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
        tx_cmd_buff_o->data[OPCODE_INDEX] = COMMON_ACK_OPCODE;
        break;
      case COMMON_NACK_OPCODE:
        tx_cmd_buff_o->data[MSG_LEN_INDEX] = ((uint8_t)0x06);
        tx_cmd_buff_o->data[OPCODE_INDEX] = COMMON_NACK_OPCODE;
        break;
      default:
        break;
    }
    tx_cmd_buff_o->end_index = // +((uint8_t)0x03) accounts for 1st 3 bytes
     (tx_cmd_buff_o->data[MSG_LEN_INDEX]+((uint8_t)0x03));
    tx_cmd_buff_o->empty = 0;
    clear_rx_cmd_buff(rx_cmd_buff_o);
  }
}

//// Attempts to pop byte from beginning of tx_cmd_buff
uint8_t pop_tx_cmd_buff(tx_cmd_buff_t* tx_cmd_buff_o) {
  uint8_t b = 0;
  if(
   !tx_cmd_buff_o->empty &&
   tx_cmd_buff_o->start_index<tx_cmd_buff_o->end_index
  ) {
    b = tx_cmd_buff_o->data[tx_cmd_buff_o->start_index];
    tx_cmd_buff_o->start_index += 1;
  }
  if(tx_cmd_buff_o->start_index==tx_cmd_buff_o->end_index) {
    clear_tx_cmd_buff(tx_cmd_buff_o);
  }
  return b;
}