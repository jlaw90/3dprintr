/*
* GCodeCommand.h
*
* Created: 06/06/2013 15:57:17
* Author: James
*/


#ifndef __GCODECOMMAND_H__
#define __GCODECOMMAND_H__

#include "../machine.h"
#include "parameter.h"
#include "../util/list.h"
#include "program.h"

namespace GCode {
  class Command;
  class Line;
  typedef void (*CommandFunction)(Machine*, Program *, Line *, Command *);

	class Command
	{
		//variables
		public:
    const char *debug_name;
    List<Parameter> params;
		protected:
		private:

		//functions
		public:
		Command(const char *debug_name, CommandFunction func = NULL);
		~Command();
		
		CommandFunction execute;

    static Command * parse(char * idx);
		protected:
		private:
		Command( const Command &c );
		Command& operator=( const Command &c );
    static Command * parse_g_code( char * idx, uint8_t num);
    static Command * parse_m_code( char * idx, uint8_t num);
	}; //GCodeCommand
}

#endif //__GCODECOMMAND_H__
