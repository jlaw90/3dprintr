/*
* Parameter.cpp
*
* Created: 06/06/2013 17:21:03
* Author: James
*/


#include "parameter.h"

namespace GCode {
  Parameter::Parameter(char type, double val)
  {
    this->type = type;
    this->val = val;
  }

  Parameter::Parameter() {}

  // default destructor
  Parameter::~Parameter()
  {
  } //~Parameter
}
