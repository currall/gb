// memory.h
#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

typedef struct {
	
    uint8_t memory[0x10000];
	uint8_t boot_rom[0x100];
	
	uint8_t boot_rom_enabled;
	
} Memory;

void mem_init(Memory* m);
void write8(Memory* m, uint16_t addr, uint8_t value);
uint8_t read8(Memory* m, uint16_t addr);

#endif