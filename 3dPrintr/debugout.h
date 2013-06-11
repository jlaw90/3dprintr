/*
* debugout.h
*
* Created: 05/06/2013 18:06:22
*  Author: James
*/


#ifdef DEBUG
#ifndef DEBUGOUT_H_
#define DEBUGOUT_H_

static int uart_putchar(char c, FILE *stream) {
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = c;
  return 0;
}

static FILE *uartstdout = fdevopen(uart_putchar, NULL);


#endif /* DEBUGOUT_H_ */
#endif /* DEBUG */