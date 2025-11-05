// cpu.h
#ifndef CPU_H
#define CPU_H

#include "registers.h"
#include "rom.h"

/* 
-- operand lengths --
number of bytes following the opcode to be used as operands
0: opcode + operands = 1 byte
1: opcode + operands = 2 bytes (8-bit  operand)
2: opcode + operands = 3 bytes (16-bit operand)
9: invalid instruction (should never be used)
*/
int operand_lengths[256] = {
	0, 2, 0, 0, 0, 0, 1, 0, 2, 0, 0, 0, 0, 0, 1, 0, // 0x0x
	1, 2, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, // 0x1x
	1, 2, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, // 0x2x
	1, 2, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, // 0x3x
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x4x
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x5x
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x6x
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x7x
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x8x
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x9x
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xAx
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xBx
	0, 0, 2, 2, 2, 0, 1, 0, 0, 0, 2, 0, 2, 2, 1, 0, // 0xCx
	0, 0, 2, 9, 2, 0, 1, 0, 0, 0, 2, 9, 2, 9, 1, 0, // 0xDx
	1, 0, 1, 9, 9, 0, 1, 0, 1, 0, 2, 9, 9, 9, 1, 0, // 0xEx
	1, 0, 1, 0, 9, 0, 1, 0, 1, 0, 2, 0, 9, 9, 1, 0  // 0xFx
};

void cycle(uint8_t* m, Registers reg);

#endif