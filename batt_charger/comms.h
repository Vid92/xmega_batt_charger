#ifndef COMMS_H_
#define COMMS_H_
#include "control.h"
#include "cfeeprom.h"
#include "program.h"
#include "Timer.h"
#include "globals.h"
#include "crc.h"

extern char rcvchar;
extern bool flagcommand;
extern bool flagcomms;
extern bool flagbuff;
extern bool flagtime;
//extern Control control;
extern Program program;
extern Timer to;

void comms_inicbuff(void);        // Borra buffer
int comms_addcbuff(char c);       // añade carácter recibido al buffer
void comms_procesa_comando(void); // Procesa comando

#endif
