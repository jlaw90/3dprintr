/*
* GCodeCommand.h
*
* Created: 06/06/2013 15:57:17
* Author: James
*/


#ifndef __GCODECOMMAND_H__
#define __GCODECOMMAND_H__

#include "../Machine.h"
namespace GCode {
	class Command
	{
		//variables
		public:
		protected:
		private:

		//functions
		public:
		Command();
		~Command();
		
		void (Command::*execute)(Machine* , Command*);
		protected:
		private:
		Command( const Command &c );
		Command& operator=( const Command &c );

	}; //GCodeCommand
}

#endif //__GCODECOMMAND_H__
