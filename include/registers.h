// registers.h
#ifndef REGISTERS_H
#define REGISTERS_H

#include "rom.h"

typedef struct {

    union { struct { uint8_t F, A; }; uint16_t AF; };
    union { struct { uint8_t C, B; }; uint16_t BC; };
    union { struct { uint8_t E, D; }; uint16_t DE; };
    union { struct { uint8_t L, H; }; uint16_t HL; };
	uint16_t	SP;
	uint16_t 	PC;

} Registers;

#endif