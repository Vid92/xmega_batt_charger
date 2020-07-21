#ifndef CFEEPROM_H_
#define CFEEPROM_H_
//#include "EEPROM.h"
//#include "I2CEEPROM.h"
#include "extEeprom.h"
#include <ArduinoJson.h>
#include "globals.h"
#include "crc.h"

extern char anbu[1024];
extern char add[0];
extern char type[17][20];
extern unsigned long duration[17];
extern float AmperH[17];
extern float current[17];
extern float maxtemp[17];
extern float mintemp[17];
extern unsigned int myaddress;
extern unsigned long totalTime;

extern bool flagload;
extern int LedComms;

extern int disk1;
extern int disk2;
extern bool flagpause;
extern int x;

void eepromsave(char* anbu);
//void writeAddress(char* add);
char* IDread(void);
char* eepromread(void);
//char* readAddress(void);
void clearProgram(void);
void loadProgram(void);
//extern char data_crc(String val);

#endif
