/*
* Bootloader.cpp
*
* Created: 12/06/2013 23:40:59
*  Author: James
*/
#define F_CPU 20000000UL
#define BAUD_RATE 28800UL

#include <util/delay.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/boot.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "bootloader.h"

// Write for printf (send via serial)
static int uart_putchar(char c, FILE *stream) {
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = c;
  return 0;
}

// Configure serial
void configure_usart() {
  UBRR0 = (F_CPU / (8UL * BAUD_RATE)) - 1UL; // baud rate
  UCSR0A |= _BV(U2X0);
  UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0); // Enable tx, rx, rx interrupts
  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); // 8-bit data
}

// Send a packet
static void send(const char *c) {
  printf("%s\r\n", c);
}

void announce_fw() {
  eeprom_busy_wait();
  volatile uint8_t fw_major = eeprom_read_byte(ADDR_FW_MAJOR);
  volatile uint8_t fw_minor = eeprom_read_byte(ADDR_FW_MINOR);
  printf("FW:");
  if(fw_major == 0xff && fw_minor == 0xff)
    printf("blank");
  else
    printf("%d.%03d", fw_major, fw_minor);
  printf("\r\n");
}

// Announce bootloader and fw version
void announce() {
  send("Bootr v1.000");
  send("READY");
}

// Jump to program
void run_application() {
  // Todo: reset peripherals

  // Put interrupts back in app land
  MCUCR = (1 << IVCE);
  MCUCR = 0;
  asm("jmp 0000");
}

// Entry point
int main(void) {
  // Check eeprom for boot flag
  uint8_t b = eeprom_read_byte(ADDR_APP_RUN);
  if(b) { // If set, run application
    // Clear incase app fails
    eeprom_write_byte(ADDR_APP_RUN, 0);
    // Jump to program...
    run_application();
  }

  // Try and boot program on reboot...
  eeprom_write_byte(ADDR_APP_RUN, 0xff);

  // Set interrupt vectors to bootloader interrupt table...
  MCUCR = (1 << IVCE);
  MCUCR = (1 << IVSEL);

  configure_usart(); // Set up USART
  stdout = fdevopen(uart_putchar, NULL);

  sei(); // Enable interrupts...
  announce();
  while(1);
}

void set_ver(char major, char minor) {
  eeprom_busy_wait();
  eeprom_write_byte(ADDR_FW_MAJOR, major);
  eeprom_busy_wait();
  eeprom_write_byte(ADDR_FW_MINOR, minor);
}

// Handle packets
#define PKT_CMDS 0
#define PKT_WRITE 1

char new_maj;
char new_min;
char pkt_mode = PKT_CMDS;
uint32_t last_page;
void ParsePacket(char *idx, unsigned char len) {
  if(pkt_mode == PKT_CMDS) {
    if(strcmp(idx, "HELLO") == 0 && len == 5) {// announce
      announce();
      return;
    }

    if(*idx == 1 && len == 1) { // To command mode, ignore...
      send("OK");
    } else if(strcmp(idx, "BOOT") == 0 && len == 4) { // BOOT
      send("OK");
      run_application();
    } else if(strcmp(idx, "FWVER") == 0 && len == 5) {
      announce_fw();
      send("OK");
    } else if(strcmp(idx, "WRITE") == 0 && len == 5) {
      pkt_mode = PKT_WRITE;
      last_page = 0xffffffff;
      new_maj = 0xff;
      new_min = 0xfe;
      set_ver(0xff, 0xfe); // Mark as unknown/corrupted

      send("OK");
    } else
      printf("E:Unknown '%s', len: %u\r\n", idx, len);
  } else if(pkt_mode == PKT_WRITE && len >= 1) {
    len--;
    char flag = *idx++;
    if(flag == 0 && len > 4) { // Data following...
      uint32_t addr = *((uint32_t *) idx);
      idx += 4;
      uint32_t page = addr / SPM_PAGESIZE;
      if(page != last_page) {
        if(last_page != 0xffffffff) {
          boot_page_write_safe(last_page * (uint32_t) SPM_PAGESIZE);
        }
        boot_page_erase_safe(addr);
      }
      last_page = page;
      len -= 4;
      for(char i = 0; i < len; i += 2) {
        uint16_t w = *idx++;
        w += (*idx++) << 8;
        boot_page_fill_safe(addr + i, w);
      }
      send("OK");
    } else if(flag == 1 && len == 0) { // Discard, to command mode
      pkt_mode = PKT_CMDS;
      send("OK");
    } else if(flag == 2 && len == 2) { // Set version for new fw
      new_maj = *idx++;
      new_min = *idx++;
      send("OK");
    } else if(flag == 3 && len == 0) { // All sent, flush and back to cmd mode
      if(last_page != 0xffffffff) {
        boot_page_write_safe(last_page * (uint32_t) SPM_PAGESIZE);
      }
      boot_spm_busy_wait();
      set_ver(new_maj, new_min);
      boot_rww_enable_safe();
      pkt_mode = PKT_CMDS;
      send("OK");
    } else {
      printf("E:Invalid flag '%c', data: ", flag);
      for(unsigned char i = 0; i < len; i++)
        printf("%02X", idx[i]);
      printf("\r\n");
    }
  } else {
    send("E:Invalid packet");
  }
}

// Protocol handler
char buf[256];
unsigned char bufoff = 0;
#define READ_STATE_P 0
#define READ_STATE_LEN 1
#define READ_STATE_DATA 2
#define READ_STATE_CHECKSUM 3
#define READ_STATE_ENDA 4
#define READ_STATE_ENDB 5
char read_state = READ_STATE_P;
char data_len, data_rem;
char checksum;
char checksum_match;
ISR(USART0_RX_vect) {
  char c = UDR0;
  switch(read_state) {
    case READ_STATE_P:
      if(c != 'P')
        send("E:Invalid packet start");
      else {
        bufoff = 0;
        read_state = READ_STATE_LEN;
        checksum = c;
      }
      break;
    case READ_STATE_LEN:
      data_len = data_rem = c;
      checksum += c;
      read_state = READ_STATE_DATA;
      break;
    case READ_STATE_DATA:
      buf[bufoff++] = c;
      checksum += c;
      if(data_rem-- == 0)
        read_state = READ_STATE_CHECKSUM;
      break;
    case READ_STATE_CHECKSUM:
      checksum ^= 0xff;
      checksum_match = c == checksum;
      if(!checksum_match)
        send("E:Checksum mismatch");
      read_state = READ_STATE_ENDA;
      break;
    case READ_STATE_ENDA:
      if(c != 0) {
        send("E:0 1");
        checksum_match = 0;
      }
      buf[bufoff++] = 0;
      read_state = READ_STATE_ENDB;
      break;
    case READ_STATE_ENDB:
      if(c != 0) {
        send("E:0 2");
        checksum_match = 0;
      }
      if(checksum_match)
        ParsePacket(&buf[0], data_len);
      read_state = READ_STATE_P;
      break;
  }
}