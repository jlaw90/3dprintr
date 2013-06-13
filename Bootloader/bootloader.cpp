/*
 * Bootloader.cpp
 *
 * Created: 12/06/2013 23:40:59
 *  Author: James
 */ 
 #define F_CPU 20000000UL
 #define BAUD_RATE 28800UL

#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "bootloader.h"

void run_application() {
  asm("jmp 0000"); // Set PC to 0
  // We shouldn't need to clear any registers as we haven't done anything at this stage...
  // If running the application after a firmware update, might be easier to trigger watchdog reset
}

static int uart_putchar(char c, FILE *stream) {
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = c;
  return 0;
}

void configure_usart()
{
  UBRR0 = (F_CPU / (8UL * BAUD_RATE)) - 1UL; // baud rate
  UCSR0A |= _BV(U2X0);
  UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0); // Enable tx, rx, rx interrupts
  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); // 8-bit data
}

int main(void)
{
    // Todo: check eeprom for bootloader enable flag?
    uint8_t b = eeprom_read_byte(ADDR_APP_RUN);
    if(b) {
      // Clear incase app is faulty
      eeprom_write_byte(ADDR_APP_RUN, 0);
      // Jump to program...
      run_application();
      return 0; // Probably not needed...
    }

    // Set interrupt vectors to bootloader interrupt table...
    MCUCR = _BV(IVCE);
    MCUCR = _BV(IVSEL);

    configure_usart(); // Set up USART
    stdout = fdevopen(uart_putchar, NULL);

    sei(); // Enable interrupts...
    printf("Bootloader v1\r\n");
    uint8_t fw_major = eeprom_read_byte(ADDR_FW_MAJOR);
    uint8_t fw_minor = eeprom_read_byte(ADDR_FW_MINOR);
    uint32_t fw_cs = eeprom_read_dword(ADDR_FW_CHECKSUM);
    printf("Firmware status: ");
    if(fw_major == 0xff && fw_minor == 0xff)
      printf("blank");
    else
      printf("installed, version %d.%03d, checksum=0x%08lX\r\n", fw_major, fw_minor, fw_cs);
    while(1){}
}