/*
* StepperMotor.cpp
*
* Created: 05/06/2013 19:22:54
* Author: James
*/
#include "stepper_motor.h"

StepperMotor::StepperMotor(volatile unsigned char *port, uint8_t pos, uint8_t mask, uint8_t steps_per_rotation,
char *steps, uint8_t num_steps, double pitch)
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

void StepperMotor::set_step(int16_t step) {
  this->step = step;
}

int16_t StepperMotor::closest_step(double pos) {
  return (int16_t) (pos / movement_per_step());
}

void StepperMotor::set_position(double pos) {
  set_step(closest_step(pos));
}

double StepperMotor::movement_per_step() {
  return this->pitch / ((double) this->steps_per_rotation);
}

void StepperMotor::step_toward(int16_t step) {
  if(this->step == step)
  return;
  if(this->step < step)
  this->step_forward();
  else
  this->step_backward();
}

double StepperMotor::get_position() {
  return (((double) this->step) * this->pitch) / ((double) this->steps_per_rotation);
}

void StepperMotor::write(char val) {
  *port = (*port & ~mask) | ((val & mask) << pos);
}