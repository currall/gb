// cb.h
#ifndef CB_H
#define CB_H

#include "registers.h"
#include "rom.h"

void execute_cb(uint8_t opcode, Registers* reg, uint8_t* m, int* cycles);

#endif