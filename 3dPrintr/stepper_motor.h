/* 
* StepperMotor.h
*
* Created: 05/06/2013 19:22:54
* Author: James
*/
#ifndef __STEPPERMOTOR_H__
#define __STEPPERMOTOR_H__


class StepperMotor
{
public:
	volatile unsigned char *port;
	unsigned char pos;
	char mask;
	unsigned char steps_per_rotation;
	char *steps;
	unsigned char num_steps;
	int step;
	double pitch;
	
protected:
private:

public:
	StepperMotor(volatile unsigned char *port, unsigned char pos, char mask, unsigned char steps_per_rotation, char *steps, unsigned char num_steps, float pitch);
	~StepperMotor();
	
	void step_forward();
	void step_backward();
	void flush();
protected:
private:
	StepperMotor( const StepperMotor &c );
	StepperMotor& operator=( const StepperMotor &c );
	void write(char c);

};

#endif
