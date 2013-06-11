/*
* GCodeLine.cpp
*
* Created: 06/06/2013 15:47:13
* Author: James
*/


#include "line.h"
#include <stdlib.h>
#include <stdio.h>
#include "../new.h"

namespace GCode {
  // default constructor
  Line::Line()
  {
    command = 0;
  } //GCodeLine

  // default destructor
  Line::~Line()
  {
    for(uint16_t i = 0; i < commands.length(); i++)
      free(commands[i]);
  } //~GCodeLine

  Line * Line::parse(char *line) {
    Line *gcl = new Line();
    char *idx = line;
    while(*idx) {
      char type = *idx++;
      if(type == ' ') // Skip whitespace
      continue;
      if(type == ';') // comment following, break.
        break;
      if(type == '*') {// Checksum
        char ex = checksum(line);
        char sd = (char) strtod(idx, &idx);
        if(sd != ex) {
          printf("ERR: Checksum mismatch\n");
          delete gcl;
          return NULL;
        }
        } else if(type == 'N') { // Line number
        gcl->number = (uint16_t) strtod(idx, &idx);
        } else {
        idx--;
        Command *c = Command::parse(&idx);
        if(c != NULL) {
          // Parse parameters...
          Parameter *p = NULL;
          do {
            p = Parameter::parse(&idx);
            if(p != NULL)
              c->params.add(p);
          } while (p != NULL);
          gcl->commands.add(c);
        }
      }
    }
    
    return gcl;
  }

  char Line::checksum(char *line) {
    char cs = 0;
    for(unsigned char i = 0; line[i] != '*' && line[i] != 0; i++)
    cs ^= line[i];
    return cs;
  }
}