/* 
* StepperMotor.cpp
*
* Created: 05/06/2013 19:22:54
* Author: James
*/
#include "stepper_motor.h"

StepperMotor::StepperMotor(volatile unsigned char *port, unsigned char pos, char mask, unsigned char steps_per_rotation,
                           char *steps, unsigned char num_steps, double pitch)
{
	this->port = port;
	this->pos = pos;
	this->mask = mask;
	this->steps_per_rotation = steps_per_rotation;
	this->steps = steps;
	this->num_steps = num_steps;
	this->pitch = pitch;
	this->step = 0;
	
}

void StepperMotor::step_forward() {
	step++;
	flush();
}

void StepperMotor::step_backward() {
	step--;
	flush();
}

void StepperMotor::flush() {
	int step = this->step % num_steps;
	if(step < 0)
	    step = num_steps + step;
    write(steps[step]);
}

void StepperMotor::write(char val) {
	*port = (*port & ~mask) | ((val & mask) << pos);
}
