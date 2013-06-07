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
	} //GCodeLine

	// default destructor
	Line::~Line()
	{
	} //~GCodeLine

	Line * Line::parse(char *line) {
		Line *gcl = new Line();
    char *idx = line;
		while(*idx) {
      char type = *idx++;
			if(type == ' ') // Skip whitespace
			  continue;
			if(type == '*') {// Checksum
        char ex = checksum(line);
        char sd = (char) strtod(idx, &idx);
        if(sd != ex) {
          printf("ERR: Checksum mismatch");
        }
      } else if(type == 'N') { // Line number
        gcl->number = (uint16_t) strtod(idx, &idx);
      } else {
        Command *c = Command::parse(idx);
        if(c != NULL)
          gcl->commands.add(c);
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