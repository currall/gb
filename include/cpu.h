// cpu.h
#ifndef CPU_H
#define CPU_H

#include "memory.h"
#include "registers.h"
#include "rom.h"

void set_flag(int flag, Registers* reg, int enabled);
int get_flag(int flag, Registers* reg);

void cpu_init(Registers* reg);
int cpu_step(Memory* m, Registers* reg);

#endif