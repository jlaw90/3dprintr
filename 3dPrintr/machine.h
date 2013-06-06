/* 
* Machine.h
*
* Created: 06/06/2013 10:53:35
* Author: James
*/


#ifndef __MACHINE_H__
#define __MACHINE_H__

#include "stepper_motor.h"

class Machine
{
//variables
public:
  StepperMotor *x;
  StepperMotor *y;
  StepperMotor *z;
protected:
private:

//functions
public:
	Machine(StepperMotor *x, StepperMotor *y, StepperMotor *z);
	~Machine();
protected:
private:
	Machine( const Machine &c );
	Machine& operator=( const Machine &c );

}; //Machine

#endif //__MACHINE_H__
