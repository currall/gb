// rom.h
#ifndef ROM_H
#define ROM_H

#include <stdint.h>
#include "header.h"

void read_ROM(char* file, Header h, uint8_t* m);
void read_boot_ROM(char* file, Header h, uint8_t* m);
void print_ROM(uint8_t* m);

#endif