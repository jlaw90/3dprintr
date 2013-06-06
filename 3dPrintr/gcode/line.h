/*
* GCodeLine.h
*
* Created: 06/06/2013 15:47:13
* Author: James
*/


#ifndef __GCODELINE_H__
#define __GCODELINE_H__

namespace GCode {
	class Line
	{
		//variables
		public:
		protected:
		private:

		//functions
		public:
		Line();
		~Line();
		
		static Line * parse(char *line);
		protected:
		private:
		Line( const Line &c );
		Line& operator=( const Line &c );
		
		char checksum(char *line);

	}; //GCodeLine
}

#endif //__GCODELINE_H__
