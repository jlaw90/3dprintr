/*
* StepperMotor.h
*
* Created: 05/06/2013 19:22:54
* Author: James
*/
#ifndef __STEPPERMOTOR_H__
#define __STEPPERMOTOR_H__

#include <stdint.h>

class StepperMotor
{
  public:
  volatile unsigned char *port;
  uint8_t pos;
  uint8_t mask;
  uint8_t steps_per_rotation;
  char *steps;
  uint8_t num_steps;
  int16_t step;
  double pitch;
  
  protected:
  private:

  public:
  StepperMotor(volatile unsigned char *port, uint8_t pos, uint8_t mask, uint8_t steps_per_rotation, char *steps, uint8_t num_steps, double pitch);
  ~StepperMotor();
  
  void step_forward();
  void step_backward();
  void flush();
  void step_toward(int16_t step);
  void set_step(int16_t step);
  int16_t closest_step(double pos);
  void set_position(double pos);
  double movement_per_step();
  double get_position();
  protected:
  private:
  StepperMotor( const StepperMotor &c );
  StepperMotor& operator=( const StepperMotor &c );
  void write(char c);
};

#endif
