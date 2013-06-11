/*
* GCodeProgram.cpp
*
* Created: 06/06/2013 10:52:31
* Author: James
*/


#include "program.h"

#define INCHES_PER_MM 0.0393700787402

namespace GCode {
  // default constructor
  Program::Program(Machine *machine)
  {
    this->measurement_mode = MEASUREMENT_MM;
    this->movement_mode = MOVEMENT_ABS;
    this->stop_mode = STOP_PAUSE;
    this->machine = machine;
  } //GCodeProgram

  void Program::set_position(double *x, double *y, double *z, double *e) {
    if(x != NULL && this->machine->x != NULL)
    this->machine->x->set_position(this->position_to_absmm(this->machine->x, *x));
    if(y != NULL && this->machine->y != NULL)
    this->machine->y->set_position(this->position_to_absmm(this->machine->y, *y));
    if(z != NULL && this->machine->z != NULL)
    this->machine->z->set_position(this->position_to_absmm(this->machine->z, *z));
    // Todo: extruder has a separate absolute toggle :S
    if(e != NULL && this->machine->e != NULL)
    this->machine->e->set_position(this->position_to_absmm(this->machine->e, *e));
  }

  double Program::position_to_absmm(StepperMotor *axis, double pos) {
    double res = pos;
    if(this->measurement_mode == MEASUREMENT_INCHES)
      res *= INCHES_PER_MM;
    if(this->movement_mode == MOVEMENT_REL)
      res += axis->get_position();
    return res;
  }

  // default destructor
  Program::~Program()
  {
  } //~GCodeProgram
}