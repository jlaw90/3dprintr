#define F_CPU 20000000UL
#define BAUD_RATE 28800UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "debugout.h"
#include "stepper_motor.h"

void configure_usart()
{
  UBRR0 = (F_CPU / (8UL * BAUD_RATE)) - 1UL; // baud rate
  UCSR0A |= _BV(U2X0);
  UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0); // Enable tx, rx, rx interrupts
  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); // 8-bit data
}

void configure_timer() {
  TCCR1B |= _BV(CS10) | _BV(WGM12); // Same as clock
  OCR1A = F_CPU / 1000UL; // 1ms..
  TIMSK1 = _BV(OCIE1A);
}

StepperMotor *x;
char moving = 0;
char cw = 1;
char delay = 20;

int main(void)
{
  configure_timer();
  configure_usart();
  sei();
  
  DDRA = 0b11110000;
  
  // Clear screen (PuTTY)
  printf("%c", 27);
  printf("[2J"); // Clear screen
  printf("%c", 27);
  printf("[H"); // Home
  
  printf("3dprintr\r\n");
  printf("Press 's' to start and stop the spindle\r\n");
  printf("      'r' to reverse the rotation direction\r\n");
  printf("      'i' to increase rotation speed\r\n");
  printf("      'd' to decrease rotation speed\r\n");
  
  char steps[4] = {0b1001, 0b1100, 0b0110, 0b0011};
  
  x = new StepperMotor(&PORTA, 0, 0b1111, 48, &steps[0], 4, 1.25d);
  while(1) {
    double time_per_rotation = ((double) (delay * x->steps_per_rotation));
    double mmps = (1000.0d * x->pitch) / time_per_rotation;
    double rpm = 60000.0d / time_per_rotation;
    printf("DEBUG: Delay: %04d, RPM: %06.2f (%06.2f mm/s), Step: %04d, Rotations: %04d\r", delay, rpm, mmps,  x->step, x->step/x->steps_per_rotation);
  }
}

// Interrupt for USART receive (RS232)
ISR(USART0_RX_vect) {
  char c = UDR0;
  switch(c) {
    case 'r':
    cw = !cw;
    break;
    case 's':
    moving = !moving;
    break;
    case 'i':
    delay -= 1;
    break;
    case 'd':
    delay += 1;
    break;
  }
}

void move_motors() {
  if(moving) {
    if(cw) {
      x->step_forward();
      } else {
      x->step_backward();
    }
  }
}

// Interrupt handler for timer output compare a
unsigned long _millis = 0;
ISR(TIMER1_COMPA_vect) {
  _millis++;
  if(_millis >= delay) {
    _millis -= delay;
    move_motors();
  }
}

// C++ new operator
void * operator new(size_t size)
{
  return malloc(size);
}

// C++ delete operator
void operator delete(void * ptr)
{
  free(ptr);
}