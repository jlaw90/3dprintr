/*
 * serial.h
 *
 * Created: 16/06/2013 23:55:29
 *  Author: James
 */ 


#ifndef SERIAL_H_
#define SERIAL_H_

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

// Send a string
void uart_write(const char * c) {
  while(c++) {
    uart_putchar(*c, NULL);
  }
}

// Send a line (CRLF)
void uart_writeline(const char * c) {
  uart_write(c);
  uart_write("\r\n");
}

#endif /* SERIAL_H_ */