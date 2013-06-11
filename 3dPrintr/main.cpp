#define F_CPU 20000000UL
#define BAUD_RATE 28800UL
// How often we move the motors, adjust variables, etc.
#define PROCESS_INTERVAL 10

#define MODE_NORMAL 1
#define MODE_DEBUG_X 2

#define MODE MODE_NORMAL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "new.h"
#include "machine.h"
#include "stepper_motor.h"
#include "debugout.h"
#include "gcode/program.h"
#include "gcode/line.h"

using namespace GCode;

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

void configure_ports() {
  DDRA = 0b11110000;
}

Machine *machine;
Program *program;

uint16_t process_interval = PROCESS_INTERVAL;

int main(void)
{
  configure_timer();
  configure_usart();
  configure_ports();
  sei();

  printf("3dprintr - dev version\r\n");

  #ifdef MODE == MODE_DEBUG_X
  printf("Press 's' to start and stop the spindle\r\n");
  printf("      'r' to reverse spindle direction\r\n");
  printf("      'i' to increase the spindle speed\r\n");
  printf("      'd' to decrease the spindle speed\r\n");
  #elif MODE == MODE_NORMAL
  printf("start\r\n");
  #endif

  program = NULL;
  
  char steps[4] = {0b1001, 0b1100, 0b0110, 0b0011};
  machine = new Machine(
  new StepperMotor(&PORTA, 0, 0b1111, 48, &steps[0], 4, 1.25d)
  );
  while(1) {
  }
}

void process() {
  if(program == NULL || program->currentLine == NULL)
  return;
  Line *l = program->currentLine;
  // Todo: Majority of the program goes here...
}

// Interrupt for USART receive (RS232)
char buffer[256];
unsigned char bufoff = 0;
ISR(USART0_RX_vect) {
  char c = UDR0;
  #if MODE == MODE_NORMAL
  if(c == '\n') {
    // We have a whole line!
    buffer[bufoff] = 0; // Finish C-string
    Line *l = GCode::Line::parse(&buffer[0]);
    if(l != NULL) {
      #ifdef DEBUG
      printf("Parsed line: %s\r\n", &buffer[0]);
      printf("Line no: %u, checksum: %u\r\n", l->number, l->checksum(&buffer[0]));
      for(uint16_t i = 0; i < l->commands.length(); i++) {
        Command *c = l->commands[i];
        printf("  Command: %s (implemented=%i)\r\n", c->debug_name, c->execute != NULL);
        for(uint16_t j = 0; j < c->params.length(); j++) {
          Parameter *p = c->params[j];
          printf("    Parameter: %c = %f\r\n", p->type, p->val);
        }
      }
      #endif
      if(program == NULL)
      program = new Program(machine);
      program->currentLine = l;
    }
    bufoff = 0;
    } else if(c != '\r') {
    buffer[bufoff++] = c;
  }
  #endif
}

// Interrupt handler for timer output compare a
uint16_t _millis = 0;
ISR(TIMER1_COMPA_vect) {
  _millis++;
  if(_millis >= process_interval) {
    _millis = 0;
    process();
  }
}