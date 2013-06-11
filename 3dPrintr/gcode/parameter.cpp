/*
* Parameter.cpp
*
* Created: 06/06/2013 17:21:03
* Author: James
*/


#include "parameter.h"
#include <stdlib.h>

namespace GCode {
  Parameter::Parameter(char type, double val)
  {
    this->type = type;
    this->val = val;
  }

  Parameter::Parameter() {}

  Parameter * Parameter::parse(char ** idx) {
    // Gobble whitespace...
    while(**idx && **idx == ' ')
      (*idx)++;
    char type = *((*idx)++);
    switch(type) {
      case 'G':
      case 'M':
      case 'T':
      case '*':
        (*idx)--;
        return NULL;

      default:
        double d = strtod(*idx, idx);
        return new Parameter(type, d);
    }
  }

  // default destructor
  Parameter::~Parameter()
  {
  } //~Parameter
}
