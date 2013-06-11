/*
* Machine.h
*
* Created: 06/06/2013 10:53:35
* Author: James
*/


#ifndef __MACHINE_H__
#define __MACHINE_H__

#include <stdlib.h>
#include "stepper_motor.h"

class Machine
{
  //variables
  public:
  StepperMotor *x;
  StepperMotor *y;
  StepperMotor *z;
  StepperMotor *e;
  protected:
  private:

  //functions
  public:
  Machine(StepperMotor *x=NULL, StepperMotor *y=NULL, StepperMotor *z=NULL, StepperMotor *e = NULL);
  ~Machine();
  protected:
  private:
  Machine( const Machine &c );
  Machine& operator=( const Machine &c );

}; //Machine

#endif //__MACHINE_H__
