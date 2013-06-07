/*
* Parameter.h
*
* Created: 06/06/2013 17:21:03
* Author: James
*/


#ifndef __PARAMETER_H__
#define __PARAMETER_H__

namespace GCode {
  class Parameter
  {
    //variables
    public:
      char type;
      double val;
    protected:
    private:

    //functions
    public:
    Parameter();
    Parameter(char type, double val);
    ~Parameter();
    protected:
    private:
    Parameter( const Parameter &c );
    Parameter& operator=( const Parameter &c );

  }; //Parameter
}

#endif //__PARAMETER_H__
