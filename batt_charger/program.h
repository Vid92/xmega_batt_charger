#ifndef PROGRAM_H_
#define PROGRAM_H_
#include "control.h"
#include "cfeeprom.h"
#include "StopWatch.h"
#include "globals.h"

extern Control control;
extern int count;
extern char letter;

/*
double current0 = 0;
double voltage0 = 0;
double temperature0 = 0;*/

class Program
{
  public:

  int state0 = 0;
  //int count = 0;
  int prevstate0 = 0;
  bool flagStep=false;

  void runStep();
  void stopStep();
  void nextStep();
  void pauseStep();
//  void prevStep();
//  void showCurrent();
//  void showVoltage();
//  void showTemperature();
  void process_step();
};
#endif
