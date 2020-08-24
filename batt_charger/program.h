#ifndef PROGRAM_H_
#define PROGRAM_H_
#include "control.h"
#include "cfeeprom.h"
#include "StopWatch.h"
#include "globals.h"

extern Control control;
extern int count;
extern const char* letter; //char


//extern int state0;
//extern int prevstate0;

class Program
{
  public:

  int state0 = 0;
  int prevstate0 = 0;

  void runStep();
  void stopStep();
  void nextStep();
  void pauseStep();
  void process_step();
};
#endif
