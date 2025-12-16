// cpu.h
#ifndef CPU_H
#define CPU_H

#include "registers.h"
#include "rom.h"

int cycle(uint8_t* m, Registers* reg);

#endif