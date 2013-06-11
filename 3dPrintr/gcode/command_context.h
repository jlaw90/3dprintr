/*
* CommandContext.h
*
* Created: 07/06/2013 21:29:33
* Author: James
*/


#ifndef __COMMANDCONTEXT_H__
#define __COMMANDCONTEXT_H__

namespace GCode {
class Program;
class Line;
class Command;
  typedef struct {
    Program* program;
    Line* line;
    Command* command;
  }CommandContext;
}

#endif
