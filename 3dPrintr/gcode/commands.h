/*
* commands.h
*
* Created: 07/06/2013 21:22:01
*  Author: James
*/


#ifndef COMMANDS_H_
#define COMMANDS_H_

#include "program.h"
#include "line.h"
#include "command.h"
#include <stdbool.h>

using namespace GCode;

bool inches_mode(CommandContext *context) {
  context->program->measurement_mode = MEASUREMENT_INCHES;
  return true;
}

bool mm_mode(CommandContext *context) {
  context->program->measurement_mode = MEASUREMENT_MM;
  return true;
}

bool abs_mode(CommandContext *context) {
  context->program->movement_mode = MOVEMENT_ABS;
  return true;
}

bool rel_mode(CommandContext *context) {
  context->program->movement_mode = MOVEMENT_REL;
  return true;
}

bool set_position(CommandContext *context) {
  // If no coordinates, reset all to 0
  Command *c = context->command;
  double *x = c->get_parameter('X');
  double *y = c->get_parameter('Y');
  double *z = c->get_parameter('Z');
  double *e = c->get_parameter('E');
  if(x == NULL && y == NULL && z == NULL && e == NULL) {
    double zero = 0;
    // Reset all to home...
    context->program->set_position(&zero, &zero, &zero, &zero);
  } else
  context->program->set_position(x, y, z, e);
  return true; // No waiting
}

bool change_tool(CommandContext *context) {
  Command *c = context->command;
  double *t = c->get_parameter('T');
  if(t == NULL)
    return true; // What?
  char toolidx = (char) *t;
  // Todo: change tool
  return true;
}
#endif /* COMMANDS_H_ */