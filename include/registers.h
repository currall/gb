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
	uint8_t 	IME;
	uint8_t		IME_delay;
	uint8_t		halted;
	uint8_t		halt_bug;

} Registers;

#define FLAG_Z 0x80
#define FLAG_N 0x40
#define FLAG_H 0x20
#define FLAG_C 0x10

#endif