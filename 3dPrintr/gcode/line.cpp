/*
* GCodeLine.cpp
*
* Created: 06/06/2013 15:47:13
* Author: James
*/


#include "line.h"
namespace GCode {
	// default constructor
	Line::Line()
	{
	} //GCodeLine

	// default destructor
	Line::~Line()
	{
	} //~GCodeLine

	static Line * parse(char *line) {
		Line *gcl = new Line();
		for(unsigned char idx = 0; line[idx] != 0; idx++) {
			if(line[idx] == ' ') // Skip whitespace
			continue;
			
			
		}
		
		return gcl;
	}

	char checksum(char *line) {
		char cs = 0;
		for(unsigned char i = 0; line[i] != '*' && line[i] != 0; i++)
		cs ^= line[i];
		return cs;
	}
}