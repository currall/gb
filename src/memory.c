#include <stdint.h>
#include <stdio.h>

#include "memory.h"

void mem_init(Memory* m) {
	m->memory[0xFF40] = 0x91; // lcdc
	m->memory[0xFF47] = 0xFC; // palette init
	m->memory[0xFF44] = 0; // ly
	m->boot_rom_enabled = 1;
}

void write8(Memory* m, uint16_t addr, uint8_t value) {
	
    if (addr == 0xFF50) {
        m->boot_rom_enabled = 0;
        m->memory[addr] = value;
		printf("[MEMORY] Boot ROM disabled!\n");
        return;
    }
	
	if (addr < 0x8000) {
		//printf("[MEMORY] ILLEGAL MEMORY WRITE\n");
		return;
	}
	
    m->memory[addr] = value;
}

uint8_t read8(Memory* m, uint16_t addr) {
	if (addr == 0xFF50) {
		return 0xFF;
	}
	if (addr == 0xFF40) {
		return 0x91;
	}

    if (m->boot_rom_enabled && addr < 0x0100) {
        return m->boot_rom[addr];
    }
    return m->memory[addr];
	
}
