/*
* GCodeProgram.cpp
*
* Created: 06/06/2013 10:52:31
* Author: James
*/


#include "program.h"

namespace GCode {
	// default constructor
	Program::Program()
	{
    this->measurement_mode = MEASUREMENT_MM;
    this->movement_mode = MOVEMENT_ABS;
    this->stop_mode = STOP_PAUSE;
	} //GCodeProgram

	// default destructor
	Program::~Program()
	{
	} //~GCodeProgram
}