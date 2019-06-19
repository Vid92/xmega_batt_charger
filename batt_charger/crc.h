#ifndef CRC_H
#define CRC_H

#pragma once
#include <Arduino.h>

unsigned short crc16_SingleBuf(unsigned char *buf, int len);

#endif
