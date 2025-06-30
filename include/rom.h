// rom.h
#ifndef ROM_H
#define ROM_H

#include <stdint.h>
#include "header.h"

uint8_t* readROM(char* file, Header h, uint8_t* m);

#endif