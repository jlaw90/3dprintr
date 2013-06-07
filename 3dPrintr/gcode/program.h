/*
* GCodeProgram.h
*
* Created: 06/06/2013 10:52:31
* Author: James
*/


#ifndef __GCODEPROGRAM_H__
#define __GCODEPROGRAM_H__

#define MEASUREMENT_INCHES 0
#define MEASUREMENT_MM 1

#define MOVEMENT_ABS 0
#define MOVEMENT_REL 1

#define STOP_PAUSE 0
#define STOP_STOP 1

namespace GCode {
	class Program
	{
		//variables
		public:
		char measurement_mode;
		char movement_mode;
    char stop_mode;
		
		
		protected:
		private:

		//functions
		public:
		Program();
		~Program();
		protected:
		private:
		Program( const Program &c );
		Program& operator=( const Program &c );

	}; //GCodeProgram
}

#endif //__GCODEPROGRAM_H__
