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
extern char type[17][10];
extern float duration[17]; //unsigned long
extern float AmperH[17];
extern float current[17];
extern float maxtemp[17];
extern float mintemp[17];
extern unsigned int myaddress;
extern float totalTime;
extern String totalhms;
extern String nameProg;

extern bool flagload;
extern int LedComms;

extern int disk1;
extern int disk2;
extern bool flagpause;
extern int save;
extern int read;

void eepromsave(char* anbu);
void jsonSave(char* buffer);
//void jsonOrigenSave(char* json);
//void writeAddress(char* add);
char* IDread(void);
void eepromread(void);
//char* readAddress(void);
void clearProgram(void);
void loadProgram(void);
void cleanEeprom(void);
//int aux_crc(String val);

#endif
