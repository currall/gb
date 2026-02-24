#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "header.h"
#include "memory.h"
#include "ppu.h"
#include "rom.h"

uint8_t get_mbc_type(uint8_t cartridge_type) {
    uint8_t mbc = 0;
    if (cartridge_type == 0) mbc = 0;
    if (cartridge_type >= 0x01 && cartridge_type <= 0x03) mbc = 1; // MBC1
    if (cartridge_type >= 0x05 && cartridge_type <= 0x06) mbc = 2; // MBC2
    if (cartridge_type >= 0x0F && cartridge_type <= 0x13) mbc = 3; // MBC3
    if (cartridge_type >= 0x19 && cartridge_type <= 0x1E) mbc = 5; // MBC5

    printf("Cartridge Type: 0x%x, MBC%d\n",cartridge_type,mbc);
    return mbc;
}

void mem_boot(Memory* m) {
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

    m->io[0x24] = 0x77; // nr50
    m->io[0x25] = 0xF3; // nr51
    m->io[0x26] = 0xF1; // nr52

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
    m->boot_rom_enabled = 0;
}

void mem_init(Memory* m) {

    // --- CGB ---

    m->vram_bank = 0;
    m->wram_bank = 1;
    memset(m->ppu->bg_palette, 0x00, sizeof(m->ppu->bg_palette));
    memset(m->ppu->obj_palette, 0x00, sizeof(m->ppu->obj_palette));

    // --- Joypad --- (needs initialising even if boot rom is present)
    m->io[0x00] = 0xCF;
    m->j.dpad = 0x0F;
    m->j.buttons = 0x0F;
    
    // -- MBC ---
    m->mbc_type = get_mbc_type(m->cartridge_type);
	m->mbc_bank1 = 1;
	m->mbc_bank2 = 0;
    m->mbc_ram_enable = 0;

    m->mbc1_mode = 0;

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

// === MBC FUNCTIONS ===

// -- MBC1 -- 

void mbc1_write(Memory* m, uint16_t addr, uint8_t value) {
    if (addr < 0x2000) {
        if ((value & 0x0F) == 0x0A) m->mbc_ram_enable = 1;
        else m->mbc_ram_enable = 0;
        return;
    }
    if (addr < 0x4000) { // lower 5 bits of bank no
        m->mbc_bank1 = value & 0x1F;
        if (m->mbc_bank1 == 0) m->mbc_bank1 = 1;
        return;
    }
    if (addr < 0x6000) { // upper 2 bits of bank no
        m->mbc_bank2 = value & 0x03;
        return;
    }
    if (addr < 0x8000) { // bank mode
        m->mbc1_mode = value & 0x01;
        return;
    }
    if (addr >= 0xA000 && addr <= 0xBFFF) { // external ram
        if (m->mbc_ram_enable) {

            uint8_t ram_bank_no = 0;
            if (m->mbc1_mode == 1){
                uint8_t ram_banks = m->ram_size / 0x2000;
                ram_bank_no = m->mbc_bank2;

                if (m->mbc_bank2 >= ram_banks) ram_bank_no = 0;
            }
            m->eram[(addr - 0xA000) + (0x2000 * ram_bank_no)] = value;

        }
    }
}

uint8_t mbc1_read(Memory* m, uint16_t addr) {
    // rom
    if (addr < 0x4000) {
        uint8_t bank = 0;
        if (m->mbc1_mode == 1) bank = m->mbc_bank2 << 5;
        return m->rom[addr + (0x4000 * bank)];
    }
    if (addr < 0x8000) {
        uint8_t bank = m->mbc_bank1 + (m->mbc_bank2 << 5);
        uint64_t address = bank * 0x4000 + (addr - 0x4000);
        return m->rom[address];
    }
    // mbc ram
    if (addr >= 0xA000 && addr <= 0xBFFF) { // external ram
        if (m->mbc_ram_enable) {
            uint8_t ram_bank_no = 0;
            if (m->mbc1_mode == 1){
                uint8_t ram_banks = m->ram_size / 0x2000;
                ram_bank_no = m->mbc_bank2;

                if (m->mbc_bank2 >= ram_banks) ram_bank_no = 0;
            }
            return m->eram[(addr - 0xA000) + (0x2000 * ram_bank_no)];

        } else return 0xFF;
    }
}

// -- MBC3 --

void mbc3_write(Memory* m, uint16_t addr, uint8_t value) {
    if (addr < 0x2000) {
        // ram enable
        if ((value & 0x0F) == 0x0A) m->mbc_ram_enable = 1;
        else m->mbc_ram_enable = 0;
        return;
    }
    if (addr < 0x4000) { 
        m->mbc_bank1 = value & 0x7F;
        if (m->mbc_bank1 == 0) m->mbc_bank1 = 1; 
        return;
    }
    if (addr < 0x6000) { 
        m->mbc_bank2 = value; 
        return;
    }
    if (addr < 0x8000) { // clock
        return;
    }
    
    if (addr >= 0xA000 && addr <= 0xBFFF) { 
        if (m->mbc_ram_enable) {
            if (m->mbc_bank2 <= 0x03) { // eram
                m->eram[(addr - 0xA000) + (0x2000 * m->mbc_bank2)] = value;
            } else if (m->mbc_bank2 >= 0x08 && m->mbc_bank2 <= 0x0C) { // rtc
                // add rtc here
            }
        }
    }
}

uint8_t mbc3_read(Memory* m, uint16_t addr) {
    if (addr < 0x4000) { // always bank 0
        return m->rom[addr]; 
    }
    if (addr < 0x8000) {
        uint64_t address = m->mbc_bank1 * 0x4000 + (addr - 0x4000);
        return m->rom[address];
    }
    
    // eram or rtc
    if (addr >= 0xA000 && addr <= 0xBFFF) { 
        if (m->mbc_ram_enable) {
            if (m->mbc_bank2 <= 0x03) { // eram
                return m->eram[(addr - 0xA000) + (0x2000 * m->mbc_bank2)];
            } else if (m->mbc_bank2 >= 0x08 && m->mbc_bank2 <= 0x0C) { // rtc
                return 0x00; // implement rtc
            }
        }
        return 0xFF; 
    }
    return 0xFF;
}

// -- MBC5 --

void mbc5_write(Memory* m, uint16_t addr, uint8_t value) {
    if (addr < 0x2000) {
        if ((value & 0x0F) == 0x0A) m->mbc_ram_enable = 1;
        else m->mbc_ram_enable = 0;
        return;
    }
    if (addr < 0x3000) { // lower 8 bits of rom bank
        m->mbc_bank1 = (m->mbc_bank1 & 0x100) | value;
        return;
    }
    if (addr < 0x4000) { // 9th bit of rom bank
        m->mbc_bank1 = (m->mbc_bank1 & 0x0FF) | ((value & 1) << 8);
        return;
    }
    if (addr < 0x6000) { // ram bank
        m->mbc_bank2 = value & 0x0F;
        return;
    }
    if (addr < 0x8000) { // no mode register like mbc1
        return;
    }
    
    if (addr >= 0xA000 && addr <= 0xBFFF) { 
        if (m->mbc_ram_enable) {
            m->eram[(addr - 0xA000) + (0x2000 * m->mbc_bank2)] = value;
        }
    }
}

uint8_t mbc5_read(Memory* m, uint16_t addr) {
    if (addr < 0x4000) {
        // lower bank is always 0
        return m->rom[addr]; 
    }
    if (addr < 0x8000) {
        uint64_t address = m->mbc_bank1 * 0x4000 + (addr - 0x4000);
        return m->rom[address];
    }
    
    // External RAM
    if (addr >= 0xA000 && addr <= 0xBFFF) { 
        if (m->mbc_ram_enable) {
            return m->eram[(addr - 0xA000) + (0x2000 * m->mbc_bank2)];
        } else {
            return 0xFF;
        }
    }
    return 0xFF;
}

// === DMA HELPER ===

int dma_blocks(Memory* m, uint16_t addr) {
    if (!m->dma_active) return 0;

    // only block oam writes
    if (addr >= 0xFE00 && addr <= 0xFE9F) return 1;

    return 0;
}

// === MAIN MEMORY READ AND WRITE ===

// === WRITE ===

void write_vram(Memory* m, uint16_t addr, uint8_t value, int bank){
    if (!(addr >= 0x8000 && addr <= 0x9FFF)) return;
    if (!bank == 0 && !bank == 1) bank = m->vram_bank;
    m->vram[bank][addr - 0x8000] = value;
}

void raw_write(Memory* m, uint16_t addr, uint8_t value) {

    // mbc
    if (m->mbc_type == 1) {
        if (addr < 0x8000) mbc1_write(m,addr,value); // rom write
        if (addr >= 0xA000 && addr <= 0xBFFF) mbc1_write(m,addr,value); // external ram
    }  
    else if (m->mbc_type == 2) {
        if (addr < 0x8000) mbc1_write(m,addr,value); // rom write
        if (addr >= 0xA000 && addr <= 0xBFFF) mbc1_write(m,addr,value); // external ram
    } 
    else if (m->mbc_type == 3) {
        if (addr < 0x8000) mbc3_write(m,addr,value); // rom write
        if (addr >= 0xA000 && addr <= 0xBFFF) mbc3_write(m,addr,value); // external ram / rtc
    }   
    else if (m->mbc_type == 5) {
        if (addr < 0x8000) mbc5_write(m,addr,value); // rom write
        if (addr >= 0xA000 && addr <= 0xBFFF) mbc5_write(m,addr,value); // external ram
    } 

    // vram
    if (addr >= 0x8000 && addr <= 0x9FFF) write_vram(m, addr, value, 0);

    // wram
    if (addr >= 0xC000 && addr <= 0xCFFF) m->wram[0][addr - 0xC000] = value;
    if (addr >= 0xD000 && addr <= 0xDFFF) m->wram[m->wram_bank][addr - 0xD000] = value;

    // echo ram
    if (addr >= 0xE000 && addr <= 0xEFFF) m->wram[0][addr - 0xE000] = value;
    if (addr >= 0xF000 && addr <= 0xFDFF) m->wram[m->wram_bank][addr - 0xF000] = value;

    // normal memory
    if (addr >= 0xFE00 && addr <= 0xFE9F) m->oam[addr - 0xFE00] = value;
    if (addr >= 0xFF00 && addr <= 0xFF7F) m->io[addr - 0xFF00] = value;
    if (addr >= 0xFF80 && addr <= 0xFFFE) m->hram[addr - 0xFF80] = value;
    if (addr == 0xFFFF) m->ie = value;
}

void write8(Memory* m, uint16_t addr, uint8_t value) {

    if (addr >= 0xFF00 && addr <= 0xFF7F)
        m->io[addr - 0xFF00] = value; // always store io

    if (addr == 0xFF00) { // input
        raw_write(m,0xFF00, (raw_read(m,0xFF00) & 0xCF) | (value & 0x30) );
    }

    if (addr == 0xFF02) {
        if ((value & 0x81) == 0x81) { // check bits 0 (master) and 7 (trigger)
            write8(m, 0xFF01, 0xFF); // set to all 1s
            value &= ~0x80; // clear transfer flag
            
            //serial interrupt
            uint8_t iflags = read8(m, 0xFF0F);
            write8(m, 0xFF0F, iflags | 0x08); 
        } 
        // If value == 0x80 (Slave mode), we do absolutely nothing.
        // The transfer flag stays high and no interrupt fires.
        
        raw_write(m, addr, value);
        return;
    }
	
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

    if (addr == 0xFF4F) {
        m->vram_bank = value & 0x01; // CGB vram bank no
    }
	
    if (addr == 0xFF50 && value != 0 && m->boot_rom_enabled) {
        m->boot_rom_enabled = 0;
		printf("[MEMORY] Boot ROM disabled!\n");
    }

    if (addr == 0xFF69) {
        uint8_t index = m->io[0x68] & 0x3F;

        ((uint8_t*)m->ppu->bg_palette)[index] = value;

        if (m->io[0x68] & 0x80) {
            index = (index + 1) & 0x3F;
            m->io[0x68] = (m->io[0x68] & 0x80) | index;
        }
        return;
    }
    if (addr == 0xFF6B) {
        uint8_t index = m->io[0x6A] & 0x3F;

        ((uint8_t*)m->ppu->obj_palette)[index] = value;

        if (m->io[0x6A] & 0x80) {
            index = (index + 1) & 0x3F;
            m->io[0x6A] = (m->io[0x6A] & 0x80) | index;
        }
        return;
    }

    if (addr == 0xFF70) { // CGB wram bank no
        m->wram_bank = value & 0x07;
        if (m->wram_bank == 0) m->wram_bank = 1;
    }

    // --- DMA ---
    if (dma_blocks(m, addr)) {
        return;
    }

    // === raw write === 
    raw_write(m,addr,value);
}

void write16(Memory* m, uint16_t addr, uint16_t value) {
    write8(m, addr, (uint8_t)(value & 0x00FF));
    write8(m, addr+1, (uint8_t)((value & 0xFF00) >> 8));
}

// === READ ===

uint8_t read_vram(Memory* m, uint16_t addr, int bank){
    if (!(addr >= 0x8000 && addr <= 0x9FFF)) return 0;
    if (!(bank == 0 || bank == 1)) bank = m->vram_bank;
    return m->vram[bank][addr - 0x8000];
}

uint8_t raw_read(Memory *m, uint16_t addr) {

    // echo ram
    if (addr >= 0xE000 && addr <= 0xFDFF)
        addr -= 0x2000;
        
    // boot rom
    if (m->boot_rom_enabled && addr < 0x0100) {
        return m->boot_rom[addr];
    }

    // mbc
    if (m->mbc_type == 1) {
        if (addr < 0x8000) return mbc1_read(m,addr);
        if (addr >= 0xA000 && addr <= 0xBFFF) return mbc1_read(m,addr);
    }
    else if (m->mbc_type == 2) {
        if (addr < 0x8000) return mbc1_read(m,addr);
        if (addr >= 0xA000 && addr <= 0xBFFF) return mbc1_read(m,addr);
    }
    else if (m->mbc_type == 3) {
        if (addr < 0x8000) return mbc3_read(m,addr);
        if (addr >= 0xA000 && addr <= 0xBFFF) return mbc3_read(m,addr);
    }
    else if (m->mbc_type == 5) {
        if (addr < 0x8000) return mbc5_read(m,addr);
        if (addr >= 0xA000 && addr <= 0xBFFF) return mbc5_read(m,addr);
    }
    else {
        if (addr < 0x8000) return m->rom[addr];
        if (addr >= 0xA000 && addr <= 0xBFFF) return 0xFF;
    }
    
    // vram
    if (addr >= 0x8000 && addr <= 0x9FFF) return m->vram[m->vram_bank][addr - 0x8000];

    // wram
    if (addr >= 0xC000 && addr <= 0xCFFF) return m->wram[0][addr - 0xC000];
    if (addr >= 0xD000 && addr <= 0xDFFF) return m->wram[m->wram_bank][addr - 0xD000];

    // echo ram
    if (addr >= 0xE000 && addr <= 0xEFFF) return m->wram[0][addr - 0xE000];
    if (addr >= 0xF000 && addr <= 0xFDFF) return m->wram[m->wram_bank][addr - 0xF000];

    // regular memory

    if (addr >= 0xFE00 && addr <= 0xFE9F) return m->oam[addr - 0xFE00];
    if (addr >= 0xFF00 && addr <= 0xFF7F) return m->io[addr - 0xFF00];
    if (addr >= 0xFF80 && addr <= 0xFFFE) return m->hram[addr - 0xFF80];
    if (addr == 0xFFFF) return m->ie;
    
    return 0xFF; // failsafe
}

uint8_t read8(Memory* m, uint16_t addr) {
	
	// === read rules ===

	//if (addr == 0xFF00) return 0xCF; // block input
    if (addr == 0xFF00) { // input
        uint8_t select = raw_read(m,0xFF00) & 0x30;
        uint8_t result = select | 0xC0 | 0x0F;

        if (!(select & 0x10))
            result &= (0xF0 | m->j.dpad);

        if (!(select & 0x20))
            result &= (0xF0 | m->j.buttons);

        return result;
    }

    if (addr == 0xFF01) return 0xFF;
	if (addr == 0xFF04) return m->div_counter >> 8; // div counter
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
    if (addr == 0xFF4F) return m->vram_bank | 0xFE;
    if (addr == 0xFF70) return m->wram_bank | 0xF8;
    
    // --- dma ---
	if (dma_blocks(m, addr)) {
        return 0xFF;
    }

    // === raw read ===
    return raw_read(m, addr);
	
}

uint16_t read16(Memory* m, uint16_t addr) {
    return (uint16_t)((read8(m,addr) | (read8(m,addr+1) << 8)));
}
