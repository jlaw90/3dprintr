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
#include "serial.h"
#include "bootloader.h"

// Send firmware version
void announce_fw() {
  eeprom_busy_wait();
  volatile uint8_t fw_major = eeprom_read_byte(ADDR_FW_MAJOR);
  volatile uint8_t fw_minor = eeprom_read_byte(ADDR_FW_MINOR);
  uart_write("FW:");
  if(fw_major == 0xff && fw_minor == 0xff)
    uart_write("blank");
  else {
    char data[4];
    itoa(fw_major, data, 10);
    uart_write(data);
    uart_write(".");
    itoa(fw_minor, data, 10);
    size_t len = strlen(data);
    for(char i = 0; i < 3 - len; i++)
      uart_write("0");
    uart_write(data);
  }
  uart_write("\r\n");
}

// Announce bootloader version, signal ready to receive commands
void announce() {
  uart_writeline("Bootr v1.000");
  uart_writeline("READY");
}

// Jump to program
void run_application() {
  // Todo: reset peripherals

  // Put interrupts back in app land
  MCUCR = (1 << IVCE);
  MCUCR = 0;
  asm("jmp 0000"); // EZmode
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

  configure_usart(); // Set up serial communications

  sei(); // Enable interrupts...
  announce();
  while(1);
}

// Sets the firmware version numbers in EEPROM
void set_ver(char major, char minor) {
  eeprom_busy_wait();
  eeprom_write_byte(ADDR_FW_MAJOR, major);
  eeprom_busy_wait();
  eeprom_write_byte(ADDR_FW_MINOR, minor);
}

// Packet stuff below
#define PKT_CMDS 0
#define PKT_WRITE 1
#define PKT_READ 2

char new_maj;
char new_min;
char pkt_mode = PKT_CMDS;
uint32_t last_page;

// Called by ParsePacket, handles basic commands e.g. BOOT, FWVER, SIZE, WRITE
void ParseCmdPacket(char *idx, unsigned char len) {
  if(strcmp(idx, "HELLO") == 0 && len == 5) {// announce
    announce();
    return;
  }

  if(*idx == 1 && len == 1) { // PC app sends this on connect to exit program mode, ignore when in cmd mode
    uart_writeline("OK");
  } else if(strcmp(idx, "BOOT") == 0 && len == 4) { // BOOT the program
    uart_writeline("OK");
    run_application();
  } else if(strcmp(idx, "FWVER") == 0 && len == 5) { // Send firmware version
    announce_fw();
    uart_writeline("OK");
  } else if(strcmp(idx, "SIZE") == 0 && len == 4) { // Send flash size
    uint32_t size = FLASHEND + 1;
    char *size_cp = (char *) &size;
    uart_putchar(size_cp[0], NULL);
    uart_putchar(size_cp[1], NULL);
    uart_putchar(size_cp[2], NULL);
    uart_putchar(size_cp[3], NULL);
    uart_writeline("OK");
  } else if(strcmp(idx, "WRITE") == 0 && len == 5) { // To program mode...
    pkt_mode = PKT_WRITE;
    last_page = 0xffffffff;
    new_maj = 0xff;
    new_min = 0xfe;
    set_ver(0xff, 0xfe); // Mark as unknown/corrupted

    uart_writeline("OK");
  } else {
    uart_write("Unknown cmd: ");
    uart_writeline(idx);
  }
}

// Called by ParsePacket, handles firmware updating packets
void ParseWritePacket(char *idx, unsigned char len) {
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
    uart_writeline("OK");
  } else if(flag == 1 && len == 0) { // Discard, to command mode
    pkt_mode = PKT_CMDS;
    uart_writeline("OK");
  } else if(flag == 2 && len == 2) { // Set version for new fw
    new_maj = *idx++;
    new_min = *idx++;
    uart_writeline("OK");
  } else if(flag == 3 && len == 0) { // All sent, flush and back to cmd mode
    if(last_page != 0xffffffff) {
      boot_page_write_safe(last_page * (uint32_t) SPM_PAGESIZE);
    }
    boot_spm_busy_wait();
    set_ver(new_maj, new_min);
    boot_rww_enable_safe();
    pkt_mode = PKT_CMDS;
    uart_writeline("OK");
  } else {
    uart_write("E:Invalid flag: '");
    uart_putchar(flag, NULL);
    uart_write("', data: ");
    for(unsigned char i = 0; i < len; i++) {
      char str[3];
      itoa(idx[i], str, 16);
      size_t len = strlen(str);
      for(char j = 0; j < 2 - len; j++)
        uart_write("0");
      uart_write(str);
    }
    uart_write("\r\n");
  }
}

// Called by our ISR (below) when we have received a verified packet
// The custom protocol implements checksumming so we know it's valid :D
void ParsePacket(char *idx, unsigned char len) {
  if(pkt_mode == PKT_CMDS) { // Basic commands
    ParseCmdPacket(idx, len);
  } else if(pkt_mode == PKT_WRITE && len >= 1) { // Receiving firmware
    ParseWritePacket(idx, len);
  } else {
    uart_writeline("E:Invalid packet");
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
        uart_writeline("E:Invalid packet start");
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
        uart_writeline("E:Checksum mismatch");
      read_state = READ_STATE_ENDA;
      break;
    case READ_STATE_ENDA:
      if(c != 0) {
        uart_writeline("E:0 1");
        checksum_match = 0;
      }
      buf[bufoff++] = 0;
      read_state = READ_STATE_ENDB;
      break;
    case READ_STATE_ENDB:
      if(c != 0) {
        uart_writeline("E:0 2");
        checksum_match = 0;
      }
      if(checksum_match)
        ParsePacket(&buf[0], data_len);
      read_state = READ_STATE_P;
      break;
  }
}