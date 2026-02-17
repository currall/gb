// memory.h
#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

typedef struct {

	uint8_t dpad; 	 // right left up down
	uint8_t buttons; // a b select start

} Joypad;

typedef struct Memory {

	// === MEMORY ===

	uint8_t vram[0x2000];   // 8000-9FFF
	uint8_t* eram;   		// A000-BFFF - extensible using MBC cartridges
	uint8_t wram[0x2000];   // C000-DFFF
	uint8_t oam[0xA0];      // FE00-FE9F
	uint8_t io[0x80];       // FF00-FF7F
	uint8_t hram[0x7F];     // FF80-FFFE
	uint8_t ie;             // FFFF

	// === ROM ===

	uint8_t* boot_rom;
	uint8_t boot_rom_enabled;

	uint8_t* rom;
	uint32_t rom_size;

	// === MBC ===
	uint8_t cartridge_type;
	uint8_t mbc_type;
	uint8_t mbc_bank1;
	uint8_t mbc_bank2;
	uint8_t mbc1_mode;

	// -- MBC RAM --
	uint8_t mbc_ram_enable;
	uint32_t ram_size;
	
	// === TIMERS ===
	uint16_t div_counter;
	int tima;
	
	// === DMA ===
	int dma_active;
	int dma_pending;
	uint16_t dma_source;
	uint16_t dma_index;
	int dma_cycles_left;

	// === INPUT === 
	Joypad j;
	
} Memory;

void mem_init(Memory* m);

void raw_write(Memory* m, uint16_t addr, uint8_t value);
void write8(Memory* m, uint16_t addr, uint8_t value);
void write16(Memory* m, uint16_t addr, uint16_t value);

uint8_t raw_read(Memory *m, uint16_t addr);
uint8_t read8(Memory* m, uint16_t addr);
uint16_t read16(Memory* m, uint16_t addr);

void print_memory(Memory* m);

#endif