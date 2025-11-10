// cpu.h
#ifndef CPU_H
#define CPU_H

#include "registers.h"
#include "rom.h"

void cycle(uint8_t* m, Registers* reg);

#endif