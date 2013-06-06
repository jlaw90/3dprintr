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

namespace GCode {
	class Program
	{
		//variables
		public:
		char measurement_mode = MEASUREMENT_MM;
		char movement_mode = MOVEMENT_ABS;
		
		
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
