#ifndef EXTEEPROM_H
#define EXTEEPROM_H

#pragma once
#include <Arduino.h>
#include <Wire.h>

void writeEEPROM(int deviceaddress, unsigned int eeaddress, byte data );
byte readEEPROM(int deviceaddress, unsigned int eeaddress );

#endif
