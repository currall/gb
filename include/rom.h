// rom.h
#ifndef ROM_H
#define ROM_H

#include <stdint.h>
#include "header.h"

uint32_t read_rom(char* file, uint8_t** m);
void read_boot_ROM(char* file, uint8_t* m);
void print_ROM(uint8_t* m);

#endif