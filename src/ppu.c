#include <stdio.h>
#include "ppu.h"
#include "memory.h"

// interrupt addresses

#define IF 0xFF0F
#define IE 0xFFFF

/*
Game Boy timing:
- 456 cycles per scanline
- 144 visible scanlines
- 10 vblank scanlines (144–153)
Source: https://gbdev.io/pandocs/Rendering.html
*/

uint32_t framebuffer[160 * 144];
uint32_t palette[4] = {0xFFFFFFFF, 0xFFC0C0C0, 0xFF606060, 0xFF000000};
//uint32_t palette[4] = {0xffffe5ec, 0xFFffc2d1, 0xFFc9184a, 0xFF800f2f};
uint32_t sprite_palette_transparent[4] = {0x00000000, 0xFFC0C0C0, 0xFF606060, 0xFF000000}; // replace white with transparent

void request_stat_interrupt(Memory* m) {
    uint8_t iflags = read8(m, IF);
    write8(m, IF, iflags | 0x02);
}

void ppu_mode_set(PPU* ppu, Memory* m, int mode) {
    ppu->mode = mode;

    uint8_t stat = read8(m, STAT);
    stat = (stat & ~0x03) | mode;
    write8(m, STAT, stat);

    if ((mode == MODE_HBLANK && (stat & (1<<3))) ||
        (mode == MODE_VBLANK && (stat & (1<<4))) ||
        (mode == MODE_OAM	 && (stat & (1<<5)))) {
        request_stat_interrupt(m);
    }
}

void check_lyc(PPU* ppu, Memory* m) {
    uint8_t ly  = read8(m, LY);
    uint8_t lyc = read8(m, LYC);
    uint8_t stat = read8(m, STAT);

    int equal = (ly == lyc);

    if (equal) stat |= (1<<2);
    else stat &= ~(1<<2);
    write8(m, STAT, stat);

    if (equal && (stat & (1<<6))) {
        request_stat_interrupt(m);
    }
}

void ppu_init(PPU* ppu, Memory* m) {
	for (int i = 0; i < 160 * 144; i++) framebuffer[i] = 0xFFFFFFFF;
	ppu_mode_set(ppu, m, MODE_VBLANK);
    ppu->scanline = 0;
    ppu->dot = 0;
    ppu->frame_ready = 0;
}

void draw_bg(int scanline, uint32_t* framebuffer, Memory* m) {
	/*
	screen is drawn scanline at a time.
	for each pixel find the tile being drawn and the pixel in the tile
	
	- tile x and y track which tile the emulator is drawing currently
	- pixel x and y track the current pixel inside the tile
	*/
	
	/* TEST WITH HORIZONAL LINES
	for (int x = 0; x < 160; x++) {
		framebuffer[scanline * 160 + x] = palette[(scanline / 8) % 4];
	}
	return;
	*/
	
    uint8_t scx = read8(m, SCX);
	uint8_t scy = read8(m, SCY);

	int y = (scanline + scy) & 0xFF;
	int tile_y = y / 8;
	int pixel_y = y % 8;

    for (int x = 0; x < 160; x++) {
        int x_scroll = (x + scx) & 0xFF;
		int tile_x = x_scroll / 8;
		int pixel_x = x_scroll % 8;
		
		uint16_t lcdc = read8(m, LCDC);
		
		uint16_t bg_map = (lcdc & (1 << 3)) ? 0x9C00 : 0x9800;
		tile_x &= 31;
		tile_y &= 31;
        uint16_t tile_pos = bg_map + tile_y * 32 + tile_x; // index of tile
        uint8_t tile_id = read8(m, tile_pos); 
		
        uint16_t tile; // pixel data of tile
		if (lcdc & (1 << 4)) { // unsigned
			tile = 0x8000 + tile_id * 16;
		} else { // signed
			tile = 0x9000 + ((int8_t) tile_id) * 16;
		}

        uint8_t low  = read8(m, tile + pixel_y * 2);
        uint8_t high = read8(m, tile + pixel_y * 2 + 1);

        int bit = 7 - pixel_x; // pixel within tile (flipped pixel_x)
        int color =
			((high >> bit) & 1) << 1 |
			((low  >> bit) & 1);
		int index = scanline * 160 + x;
        uint8_t bgp = read8(m, 0xFF47);
		int shade = (bgp >> (color * 2)) & 0x03;
		framebuffer[index] = palette[shade];

    }
}

void draw_fg(int scanline, uint32_t* framebuffer, Memory* m) {
	
	int sprites_drawn = 0; // gameboy has limit of 10 sprites per scanline
	
	for (int i = 0; i < 40 && sprites_drawn < 10; i++) { // 40 sprites total, 10 per scanline
		uint16_t oam = 0xFE00 + i * 4;
		
		// sprite data
		int y = read8(m, oam) - 16;
		int x = read8(m, oam + 1) - 8;
		uint8_t tile_id = read8(m, oam+2); // tile no.
		uint8_t attr = read8(m, oam+3); // attributes
		/* attributes 0xPYXT----
		P: priority - sprite drawn behind bg
		Y: vertical flip (y)
		X: horizontal flip (x)
		T: use transparent palette
		*/
		
		int h = 8; // 8x8 tiles only (gb supports 8x16)

		int tall_tile = read8(m,LCDC) & 0x04;
		if (tall_tile) h = 16;
        if (scanline < y || scanline >= y + h) continue; // skip sprite if not on scanline
		sprites_drawn++;
		
		int line = scanline - y; // line inside tile
		if (attr & 0x40) line = h - 1 - line; // vertical flip attribute
		
		uint16_t tile = 0x8000 + tile_id * 16; 
		uint8_t low  = read8(m, tile + line * 2);
		uint8_t high = read8(m, tile + line * 2 + 1);
		
		for (int px = 0; px < 8; px++) { // loop throuhg pixels horizontally (8px wide)
			
			int bit;
			if (attr & 0x20) bit= px; else bit=(7 - px); // handle horizontal flip attr
			int color = // color of pixel within tile
				((high >> bit) & 1) << 1 | 
				((low  >> bit) & 1);
			if (color == 0) continue; // hide transparent pixels
			
			int screen_x = x + px;
			if (screen_x < 0 || screen_x >= 160) continue; // skip sprite if off-screen
			
			int index = scanline * 160 + screen_x;
			if (attr & 0x80) { // sprite is behind background (priority attr)
				uint32_t bg = framebuffer[index];
				if (bg != palette[0]) // only draw if background pizel is transparent
					continue;
			}
			
			if (attr & 0x10) framebuffer[index] = sprite_palette_transparent[color]; // transparent palette
			else framebuffer[index] = palette[color]; // normal palette
			
		}
	}
}

void ppu_step(PPU* ppu, Memory* m) {

	uint8_t lcdc = read8(m, LCDC);
    
    if (!(lcdc & 0x80)) { // lcd enable
        ppu->scanline = 0;
        ppu->dot = 0;
        ppu->mode = MODE_HBLANK;
        write8(m, LY, 0); // set scanline to 0
        uint8_t stat = read8(m, STAT);
        write8(m, STAT, stat & 0xFC); 
        return;
    }
	
    ppu->dot ++;
	
	switch (ppu->mode) {
		
		case MODE_HBLANK:
		
			if (ppu->dot >= 210) { // end of hblank
				ppu->dot -= 210; // return to start of scanline

				ppu->scanline++;
				write8(m, LY, ppu->scanline); // store scanline in memory
				check_lyc(ppu,m);
				
				if (ppu->scanline == 144) {
					ppu_mode_set(ppu, m, MODE_VBLANK);
					ppu->frame_ready = 1;
					// interrupt
					uint8_t iflags = read8(m, IF);
					iflags |= (1 << 0); 
					write8(m, IF, iflags);
					
				} else {
					ppu_mode_set(ppu, m, MODE_OAM);
					ppu->frame_ready = 0;
				}
			} break;
		
		case MODE_VBLANK:
		
			if (ppu->dot >= 456) {
				ppu->dot -= 456;
				
				ppu->scanline ++;
				write8(m, LY, ppu->scanline); // increase LY counter (scanline counter)
				check_lyc(ppu,m);
				
				if (ppu->scanline > 153) {
					ppu->scanline = 0;
					write8(m, LY, ppu->scanline); // reset LY
					ppu_mode_set(ppu,m,MODE_OAM);
				}
			} break;
			
		case MODE_OAM:
		
			if (ppu->dot >= 80) {
				ppu->dot -= 80;

				if (ppu->scanline < 144) {
					draw_bg(ppu->scanline, framebuffer, m);
					draw_fg(ppu->scanline, framebuffer, m);
				}
				ppu_mode_set(ppu,m,MODE_VRAM);				
			} break;
		
		case MODE_VRAM:
		
			if (ppu->dot >= 166) {
				ppu->dot -= 166;
				ppu_mode_set(ppu,m,MODE_HBLANK);
				
			} break;
		
	}
}

int ppu_frame_ready(PPU* ppu) {
    return ppu->frame_ready;
}

uint32_t* ppu_get_framebuffer(PPU* ppu) {
    return framebuffer;
}

void clear_framebuffer() {
	for (int i = 0; i < 160 * 144; i++) framebuffer[i] = 0xFFFFFFFF; 
}