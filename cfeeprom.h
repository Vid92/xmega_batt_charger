#ifndef CFEEPROM_H_
#define CFEEPROM_H_
//#include "EEPROM.h"
#include "I2CEEPROM.h"
#include <ArduinoJson.h>
#include "globals.h"
#include "crc.h"

//#define CHIP_ADDRESS 0x50 // Address of EEPROM chip (24LC256->0x50)
//#define EEPROM_BYTES 32768 // Number of bytes in EEPROM chip

extern I2CEEPROM i2c_eeprom; // Create I2C EEPROM instance

extern char anbu[1024];
extern char add[0];
extern char type[15][20];
extern unsigned long duration[15];
extern float current[15];
extern int myaddress;

extern bool flagload;
extern int LedComms;

void eepromsave(char* anbu);
void writeAddress(char* add);
char* eepromread(void);
char* readAddress(void);
void clearProgram(void);
void loadProgram(void);

#endif
