/*
* GCodeCommand.cpp
*
* Created: 06/06/2013 15:57:17
* Author: James
*/


#include "command.h"
#include "commands.h"
#include "../util/list.h"
#include <stdio.h>
#include <stdlib.h>

namespace GCode {
  Command::Command(const char *nm, CommandFunction func)
  {
    this->debug_name = nm;
    this->execute = func;
  }

  double *Command::get_parameter(char type) {
    for(uint16_t i = 0; i < params.length(); i++)
      if(params[i]->type == type)
        return &(params[i]->val);
    return NULL;
  }

  // default destructor
  Command::~Command()
  {
    for(uint16_t i = 0; i < params.length(); i++)
      free(params[i]);
  } //~GCodeCommand

  Command * Command::parse( char ** idx )
  {
    Command *c = NULL;
    char type = *((*idx)++);
    switch(type) {
      case 'G': // G code
      c = parse_g_code((uint8_t) strtod(*idx, idx));
      break;
      case 'M': // M code
      c = parse_m_code((uint8_t) strtod(*idx, idx));
      break;
      case 'T': // T code
      c = new Command("Change tool", &change_tool);
      c->params.add(new Parameter('T', strtod(*idx, idx)));
      break;
      default:
      printf("Unknown code type: %c\r\n", type);
    }

    return c;
  }

  Command * Command::parse_g_code(uint8_t num) {
    switch(num) {
      case 0:
      return new Command("Rapid move");
      case 1:
      return new Command("Linear move");
      case 4:
      return new Command("Dwell");
      case 10:
      return new Command("Head Offset");
      case 20:
      return new Command("Inches mode", &inches_mode);
      case 21:
      return new Command("millimetres mode", &mm_mode);
      case 28:
      return new Command("Home");
      case 90:
      return new Command("Absolute positioning", &abs_mode);
      case 91:
      return new Command("Relative positioning", &rel_mode);
      case 92:
      return new Command("Set position (zero-point)", &set_position);
      default:
      printf("Unknown/Unhandled G-code: %u\r\n", num);
    }
    return NULL;
  }

  Command * Command::parse_m_code(uint8_t num) {
    switch(num) {
      case 0:
      return new Command("Stop");
      case 1:
      return new Command("Sleep");
      case 3:
      return new Command("Spindle on, CW");
      case 4:
      return new Command("Spindle on, CCW");
      case 5:
      return new Command("Spindle off");
      case 7:
      return new Command("Mist coolant on");
      case 8:
      return new Command("Flood coolant on");
      case 9:
      return new Command("Coolant off");
      case 10:
      return new Command("Vacuum on");
      case 11:
      return new Command("Vacuum off");
      case 17:
      return new Command("Enable all stepper motors (power on)");
      case 18:
      return new Command("Disable all stepper motors (power off)");
      case 40:
      return new Command("Eject");
      case 42:
      return new Command("Stop when out of material (turn off, go home, etc.)");
      case 43:
      return new Command("Standby when out of material (go home (except z) and wait), leave heated bed on");
      case 80:
      return new Command("ATX power on");
      case 81:
      return new Command("ATX power off");
      case 82:
      return new Command("Set extruder to ABS");
      case 83:
      return new Command("Set extruder to relative");
      case 84:
      return new Command("Disable idle hold");
      case 104:
      return new Command("Set extruder temperature");
      case 105:
      return new Command("Get extruder temperature");
      case 106:
      return new Command("Turn on the fan");
      case 110:
      return new Command("Set current line number (so the next expected number will be this+1)");
      case 111:
      return new Command("Set debug level 0b00, 0b01, 0b10 = echo, info, errors");
      case 112:
      return new Command("Emergency stop, don't do anything until reset is pressed");
      case 113:
      return new Command("Set extruder PWM");
      case 114:
      return new Command("Get position");
      case 115: // See: http://reprap.org/wiki/M115_Keywords
      return new Command("Get firmware version & capabilities");
      case 116:
      return new Command("Wait for temperatures and other stuff to reach their value");
      case 126:
      return new Command("Open extruder valve");
      case 127:
      return new Command("Close extruder valve");

      default:
      printf("Unknown/Unhandled M-code: %u\r\n", num);
    }
    
    return NULL;
  }
}