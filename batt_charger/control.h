#ifndef CONTROL_H_
#define CONTROL_H_

#include "xmDAC.h"
#include "StopWatch.h"
#include "globals.h"

extern StopWatch controlTime;  // solo para mostrar valores
extern bool flagStep;
extern double valcurrent;
extern double valvoltage;
extern double valtemp;
extern int LedRelay;


class Control
{
  public:

  xmDAC dac=xmDAC(xmDAC::DAC_PORT_B);
  float val_control = 0;
  unsigned long timeout = 0;
  unsigned long steptime = 0;
  int state = 0; // 1 = running, 2 = pause, 3 = stop
  int prevstate = 0;
  int valrampa = 0;

//  double valcurrent = 0;
//  double valvoltage = 0;
//  double valtemp = 0;
  //StopWatch controlTime;

//  long averageTemp = 0;

  void begin();
  void setCurrent(float val_control);
  void setTime(unsigned long timeout);
  void stepPause(unsigned long steptime);
  void run();
  void runPause();
  void pause();
  void stop();
  void event();

  void showCurrent();
  void showVoltage();
  void showTemperature();
  //void eventPause();
};

#endif
