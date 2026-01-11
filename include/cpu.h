// cpu.h
#ifndef CPU_H
#define CPU_H

#include "registers.h"
#include "rom.h"

void set_flag_z(Registers* reg, int enabled);
void set_flag_n(Registers* reg, int enabled);
void set_flag_h(Registers* reg, int enabled);
void set_flag_c(Registers* reg, int enabled);

int get_flag_z(Registers* reg);
int get_flag_n(Registers* reg);
int get_flag_h(Registers* reg);
int get_flag_c(Registers* reg);

int cycle(uint8_t* m, Registers* reg);

#endif