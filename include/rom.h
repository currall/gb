// rom.h
#ifndef ROM_H
#define ROM_H

#include <stdint.h>
#include "header.h"

typedef struct Status Status;
typedef struct Memory Memory;

char* read_rom(char* file, Memory* m, Status* s);
void read_boot_ROM(char* file, uint8_t** m);
void print_ROM(uint8_t* m);

#endif