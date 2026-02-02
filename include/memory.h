// memory.h
#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

typedef struct {

	uint8_t dpad; 	 // right left up down
	uint8_t buttons; // a b select start

} Joypad;

typedef struct {
	
    uint8_t memory[0x10000];
	uint8_t boot_rom[0x100];
	uint8_t rom[0x8000];
	
	uint8_t boot_rom_enabled;
	
	// timers
	uint16_t div_counter;
	int tima;
	
	// dma
	int dma_active;
	int dma_pending;
	uint16_t dma_source;
	uint16_t dma_index;
	int dma_cycles_left;

	// input
	Joypad j;
	
} Memory;

void mem_init(Memory* m);

void write8(Memory* m, uint16_t addr, uint8_t value);
void write16(Memory* m, uint16_t addr, uint16_t value);

uint8_t raw_read(Memory *m, uint16_t addr);
uint8_t read8(Memory* m, uint16_t addr);
uint16_t read16(Memory* m, uint16_t addr);

void print_memory(Memory* m);

#endif