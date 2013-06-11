/*
* GCodeLine.h
*
* Created: 06/06/2013 15:47:13
* Author: James
*/


#ifndef __GCODELINE_H__
#define __GCODELINE_H__

#include <stdint.h>
#include "../util/list.h"
#include "../new.h"
#include "command.h"

namespace GCode {
  class Line
  {
    //variables
    public:
    uint16_t number;
    List<Command> commands;
    uint16_t command;
    protected:
    private:

    //functions
    public:
    Line();
    ~Line();
    
    static Line * parse(char *line);

    static char checksum(char *line);
    protected:
    private:
    Line( const Line &c );
    Line& operator=( const Line &c );

  }; //GCodeLine
}

#endif //__GCODELINE_H__
