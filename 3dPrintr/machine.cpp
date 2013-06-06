/* 
* Machine.cpp
*
* Created: 06/06/2013 10:53:35
* Author: James
*/


#include "machine.h"
#include "stepper_motor.h"

// default constructor
Machine::Machine(StepperMotor *x, StepperMotor *y, StepperMotor *z)
{
	this->x = x;
	this->y = y;
	this->z = z;
} //Machine

// default destructor
Machine::~Machine()
{
} //~Machine
