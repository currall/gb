#include <stdint.h>
#include <stdio.h>

#include <string.h>

#include "memory.h"

void mem_init(Memory* m) {
	
	// --- Joypad ---
    m->memory[0xFF00] = 0xCF;

    // --- Timer ---
    m->memory[0xFF04] = 0x00; // DIV
    m->memory[0xFF05] = 0x00; // TIMA
    m->memory[0xFF06] = 0x00; // TMA
    m->memory[0xFF07] = 0xF8; // TAC (upper bits always 1)

    // --- Interrupts ---
    m->memory[0xFF0F] = 0xE1; // IF
    m->memory[0xFFFF] = 0x00; // IE

    // --- Sound ---
    m->memory[0xFF10] = 0x80;
    m->memory[0xFF11] = 0xBF;
    m->memory[0xFF12] = 0xF3;
    m->memory[0xFF14] = 0xBF;

    m->memory[0xFF16] = 0x3F;
    m->memory[0xFF17] = 0x00;
    m->memory[0xFF19] = 0xBF;

    m->memory[0xFF1A] = 0x7F;
    m->memory[0xFF1B] = 0xFF;
    m->memory[0xFF1C] = 0x9F;
    m->memory[0xFF1E] = 0xBF;

    m->memory[0xFF20] = 0xFF;
    m->memory[0xFF21] = 0x00;
    m->memory[0xFF22] = 0x00;
    m->memory[0xFF23] = 0xBF;

    m->memory[0xFF24] = 0x77;
    m->memory[0xFF25] = 0xF3;
    m->memory[0xFF26] = 0xF1; // NR52

    // --- PPU ---
    m->memory[0xFF40] = 0x91; // LCDC
    m->memory[0xFF41] = 0x85; // STAT
    m->memory[0xFF42] = 0x00; // SCY
    m->memory[0xFF43] = 0x00; // SCX
    m->memory[0xFF44] = 0x00; // LY
    m->memory[0xFF45] = 0x00; // LYC
    m->memory[0xFF47] = 0xFC; // BGP
    m->memory[0xFF48] = 0xFF; // OBP0
    m->memory[0xFF49] = 0xFF; // OBP1
    m->memory[0xFF4A] = 0x00; // WY
    m->memory[0xFF4B] = 0x00; // WX

    // --- Boot ROM ---
    m->boot_rom_enabled = 1;
	
	// --- DIV ---
	m->div_counter = 0;
	m->tima = 0;
	
	// --- DMA ---
	m->dma_active = 0;
	m->dma_pending = 0;
	m->dma_source = 0;
	m->dma_index = 0;
	m->dma_cycles_left = 0;

}

int dma_blocks2(Memory* m, uint16_t addr) {
    if (!m->dma_active) return 0;
    if (addr >= 0xFF00) return 0;
    return 1;
}

int dma_blocks(Memory* m, uint16_t addr) {
    if (!m->dma_active) return 0;

    // only block oam writes
    if (addr >= 0xFE00 && addr <= 0xFE9F) return 1;

    return 0;
}

void write8(Memory* m, uint16_t addr, uint8_t value) {
		
	//if (addr == 0xFF01) printf("[SERIAL] %c\n", value); // serial output
	
	if (addr == 0xFF04) {
		m->div_counter = 0;
		m->memory[0xFF04] = 0;
		return;
	}
	if (addr == 0xFF05) {
		m->tima = -1;
		m->memory[0xFF05] = value;
		return;
	}
	
	if (addr == 0xFF40) {
		m->memory[0xFF40] = value;
		return;
	}
	
	if (addr == 0xFF46) { // DMA
		m->dma_pending = 1;
		m->dma_source = value << 8;
	}
	
    if (addr == 0xFF50 && value != 0 && m->boot_rom_enabled) {
        m->boot_rom_enabled = 0;

		for (int i = 0; i < 0x8000; i++) {
			if (m->memory[i] != m->rom[i]) {
				printf("[ROM] ROM CORRUPTION at %04X: %02X -> %02X\n",
					   i, m->rom[i], m->memory[i]);
				//break;
			}
		}
		
        m->memory[addr] = value;
		printf("[MEMORY] Boot ROM disabled!\n");
        return;
    }
	
	if (addr < 0x8000) {
		//printf("[MEMORY] ILLEGAL MEMORY WRITE\n");
		return;
	} 
	if (dma_blocks(m, addr)) {
        return;
    }
    m->memory[addr] = value;
}

void write16(Memory* m, uint16_t addr, uint16_t value) {
    write8(m, addr, value & 0x00FF);
    write8(m, addr+1, (value & 0xFF00) >> 8);
}

uint8_t raw_read(Memory *m, uint16_t addr) {
    // echo ram
    if (addr >= 0xE000 && addr <= 0xFDFF)
        addr -= 0x2000;

    return m->memory[addr];
}

uint8_t read8(Memory* m, uint16_t addr) {
	
	// read rules
	if (addr == 0xFF00) return 0xCF;
	if (addr == 0xFF03) return 0xFF;
	if (addr >= 0xFF08 && addr <= 0xFF0E) return 0xFF;
	if (addr == 0xFF15) return 0xFF;
	if (addr == 0xFF1F) return 0xFF;
	if (addr >= 0xFF27 && addr <= 0xFF2F) return 0xFF;
	if (addr >= 0xFF4C && addr <= 0xFF7F) return 0xFF;

	if (addr == 0xFF00) return (m->memory[0xFF00] & 0x30) | 0xCF;
	if (addr == 0xFF04) return m->div_counter >> 8;
	if (addr == 0xFF07) return m->memory[0xFF07] | 0xF8;
	if (addr == 0xFF0F) return m->memory[0xFF0F] | 0xE0;
	if (addr == 0xFF26) return m->memory[0xFF26] | 0x70;
	if (addr == 0xFF41) return m->memory[0xFF41] | 0x80;
	if (addr == 0xFF44) { // LY
		if (!(m->memory[0xFF40] & 0x80))
			return 0;
	}

	if (dma_blocks(m, addr)) {
        return 0xFF;
    }
    if (m->boot_rom_enabled && addr < 0x0100) {
        return m->boot_rom[addr];
    }
    return raw_read(m, addr);
	
}

uint16_t read16(Memory* m, uint16_t addr) {
    return (uint16_t)(read8(m,addr) + (read8(m,addr+1) << 8));
}
