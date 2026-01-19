// cb.h
#ifndef CB_H
#define CB_H

#include "memory.h"
#include "registers.h"
#include "rom.h"

void execute_cb(uint8_t opcode, Registers* reg, Memory* m, int* cycles);

#endif