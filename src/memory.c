#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "header.h"
#include "memory.h"
#include "rom.h"

void mem_init(char* file, Memory* m) {
    
    Header h = read_header(file);
    print_header(h);
    
    read_boot_ROM("dmg_boot.bin",m->boot_rom); // read boot rom into boot rom memory

    m->rom_size = read_rom(file,&m->rom); // read into memory
    if (!m->rom) {
        printf("ROM load failed\n");
    }

    // -- MBC1 ---
    m->mbc1_mode = 0;
	m->mbc1_bank1 = 1;
	m->mbc1_bank2 = 0;
	
	// --- Joypad ---
    m->io[0x00] = 0xCF;
    m->j.dpad = 0x0F;
    m->j.buttons = 0x0F;

    // --- Timer ---
    m->io[0x04] = 0x00; // DIV
    m->io[0x05] = 0x00; // TIMA
    m->io[0x06] = 0x00; // TMA
    m->io[0x07] = 0xF8; // TAC (upper bits always 1)

    // --- Interrupts ---
    m->io[0x0F] = 0x00; // IF
    m->ie = 0x00; // IE

    // --- Sound ---
    m->io[0x10] = 0x80;
    m->io[0x11] = 0xBF;
    m->io[0x12] = 0xF3;
    m->io[0x14] = 0xBF;

    m->io[0x16] = 0x3F;
    m->io[0x17] = 0x00;
    m->io[0x19] = 0xBF;

    m->io[0x1A] = 0x7F;
    m->io[0x1B] = 0xFF;
    m->io[0x1C] = 0x9F;
    m->io[0x1E] = 0xBF;

    m->io[0x20] = 0xFF;
    m->io[0x21] = 0x00;
    m->io[0x22] = 0x00;
    m->io[0x23] = 0xBF;

    m->io[0x24] = 0x77;
    m->io[0x25] = 0xF3;
    m->io[0x26] = 0xF1; // NR52

    // --- PPU ---
    m->io[0x40] = 0x91; // LCDC
    m->io[0x41] = 0x85; // STAT
    m->io[0x42] = 0x00; // SCY
    m->io[0x43] = 0x00; // SCX
    m->io[0x44] = 0x00; // LY
    m->io[0x45] = 0x00; // LYC
    m->io[0x47] = 0xFC; // BGP
    m->io[0x48] = 0xFF; // OBP0
    m->io[0x49] = 0xFF; // OBP1
    m->io[0x4A] = 0x00; // WY
    m->io[0x4B] = 0x00; // WX

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

void raw_write(Memory* m, uint16_t addr, uint8_t value) {

    if (addr < 0x2000) {
        return; // ignore ram
    }
    if (addr < 0x4000) { // lower 5 bits of bank no
        m->mbc1_bank1 = value & 0x1F;
        if (m->mbc1_bank1 == 0) m->mbc1_bank1 = 1;
        printf("[MBC1] Switching to bank no.: %d\n",((m->mbc1_bank2 >> 5) + m->mbc1_bank1));
        return;
    }
    if (addr < 0x6000) { // upper 2 bits of bank no
        m->mbc1_bank2 = value & 0x03;
        printf("[MBC1] Switching to bank no. (high bank): %d\n",((m->mbc1_bank2 >> 5) + m->mbc1_bank1));
        return;
    }
    if (addr < 0x8000) { // bank mode
        m->mbc1_mode = value & 0x01;
        return;
    }

    // normal memory
    if (addr >= 0x8000 && addr <= 0x9FFF) m->vram[addr - 0x8000] = value;
    if (addr >= 0xA000 && addr <= 0xBFFF) m->eram[addr - 0xA000] = value;
    if (addr >= 0xC000 && addr <= 0xDFFF) m->wram[addr - 0xC000] = value;
    if (addr >= 0xE000 && addr <= 0xFDFF) m->wram[addr - 0xC000] = value; // echo ram
    if (addr >= 0xFE00 && addr <= 0xFE9F) m->oam[addr - 0xFE00] = value;
    if (addr >= 0xFF00 && addr <= 0xFF7F) m->io[addr - 0xFF00] = value;
    if (addr >= 0xFF80 && addr <= 0xFFFE) m->hram[addr - 0xFF80] = value;
    if (addr == 0xFFFF) m->ie = value;
}

void write8(Memory* m, uint16_t addr, uint8_t value) {

    if (addr == 0xC213) { // tetris piece memory
        printf("WRITE TO C213!! :: 0x%x | DIV :: %d\n",value,raw_read(m,0xFF04));
        raw_write(m,addr,value);
    }
    
    if (addr == 0xFF00) { // input
        raw_write(m,0xFF00, (raw_read(m,0xFF00) & 0xCF) | (value & 0x30) );
    }
		
	//if (addr == 0xFF01) printf("[SERIAL] %c\n", value); // serial output
	//if (addr == 0xFF02) printf("[SERIAL] %c\n", value); // serial output
	
	if (addr == 0xFF04) {
		m->div_counter = 0;
        raw_write(m,0xFF04,0);
		return;
	}
	if (addr == 0xFF05) m->tima = -1;
	
	if (addr == 0xFF46) { // DMA
		m->dma_pending = 1;
		m->dma_source = value << 8;
	}	
	
    if (addr == 0xFF50 && value != 0 && m->boot_rom_enabled) {
        m->boot_rom_enabled = 0;
		printf("[MEMORY] Boot ROM disabled!\n");
    }
	if (dma_blocks(m, addr)) {
        return;
    }
    raw_write(m,addr,value);
}

void write16(Memory* m, uint16_t addr, uint16_t value) {
    write8(m, addr, (uint8_t)(value & 0x00FF));
    write8(m, addr+1, (uint8_t)((value & 0xFF00) >> 8));
}

uint8_t raw_read(Memory *m, uint16_t addr) {

    // use random numebr instead of memory value, to simulate random piece - tetris
    /* if (addr == 0xC213) { // here is tetris piece
        if (m->wram[addr - 0xC000] == 0) return rand() % 29;
    } */
    

    // echo ram
    if (addr >= 0xE000 && addr <= 0xFDFF)
        addr -= 0x2000;
        
    // boot rom
    if (m->boot_rom_enabled && addr < 0x0100) {
        return m->boot_rom[addr];
    }

    // rom
    if (addr < 0x4000) {
        uint8_t bank = 0;
        if (m->mbc1_mode == 1) bank = m->mbc1_bank2 << 5;
        return m->rom[addr + (0x4000 * bank)];
    }
    if (addr < 0x8000) {
        uint8_t bank = m->mbc1_bank1 + (m->mbc1_bank2 << 5);
        uint64_t address = bank * 0x4000 + (addr - 0x4000);
        return m->rom[address];
    }

    // regular memory

    if (addr >= 0x8000 && addr <= 0x9FFF) return m->vram[addr - 0x8000];
    if (addr >= 0xA000 && addr <= 0xBFFF) return m->eram[addr - 0xA000];
    if (addr >= 0xC000 && addr <= 0xDFFF) return m->wram[addr - 0xC000];
    if (addr >= 0xE000 && addr <= 0xFDFF) return m->wram[addr - 0xE000]; // echo ram
    if (addr >= 0xFE00 && addr <= 0xFE9F) return m->oam[addr - 0xFE00];
    if (addr >= 0xFF00 && addr <= 0xFF7F) return m->io[addr - 0xFF00];
    if (addr >= 0xFF80 && addr <= 0xFFFE) return m->hram[addr - 0xFF80];
    if (addr == 0xFFFF) return m->ie;
    
    return 0xFF; // failsafe
}

uint8_t read8(Memory* m, uint16_t addr) {
	
	// read rules
	//if (addr == 0xFF00) return 0xCF; // block input
	//if (addr == 0xFF04) return m->div_counter >> 8; // div counter
	if (addr == 0xFF04) {printf("DIV : %d\n",(m->div_counter >> 8));return m->div_counter >> 8;} // div counter
    //if (addr == 0xFF04) return (uint8_t)rand(); // random value for timer
	if (addr == 0xFF07) return raw_read(m,0xFF07) | 0xF8; // timer

	if (addr >= 0xFF08 && addr <= 0xFF0E) return 0xFF;
	if (addr == 0xFF0F) return raw_read(m,0xFF0F) | 0xE0; // interrupts
	if (addr == 0xFF15) return 0xFF;
	if (addr == 0xFF1F) return 0xFF;
	if (addr == 0xFF26) return raw_read(m,0xFF26) | 0x70;
	if (addr >= 0xFF27 && addr <= 0xFF2F) return 0xFF;

	if (addr == 0xFF41) return raw_read(m,0xFF41) | 0x80;
	if (addr == 0xFF44) { // LY - scanline counter
		if (!(raw_read(m,0xFF40) & 0x80))
			return 0;
	}
	if (addr >= 0xFF4C && addr <= 0xFF7F) return 0xFF; // cgb only - blank for dmg

    if (addr == 0xFF00) { // input
        uint8_t select = raw_read(m,0xFF00) & 0x30;
        uint8_t result = select | 0xC0 | 0x0F;

        if (!(select & 0x10))
            result &= (0xF0 | m->j.dpad);

        if (!(select & 0x20))
            result &= (0xF0 | m->j.buttons);

        return result;
    }

	if (dma_blocks(m, addr)) {
        return 0xFF;
    }
    return raw_read(m, addr);
	
}

uint16_t read16(Memory* m, uint16_t addr) {
    return (uint16_t)((read8(m,addr) | (read8(m,addr+1) << 8)));
}
