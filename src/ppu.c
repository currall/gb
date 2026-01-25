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
uint32_t sprite_palette_transparent[4] = {0x00000000, 0xFFC0C0C0, 0xFF606060, 0xFF000000}; // replace white with transparent


void ppu_mode_set(PPU* ppu, Memory* m, int mode) {
	ppu->mode = mode;
	uint8_t stat = read8(m, 0xFF41);
	write8(m, 0xFF41, (stat & ~0x03) | (mode & 0x03));
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
        int color = // color of pixel within tile
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
	
    ppu->dot ++;
	
	switch (ppu->mode) {
		
		case MODE_HBLANK:
		
			if (ppu->dot >= 210) { // end of hblank
				ppu->dot -= 210; // return to start of scanline
				
				uint8_t ly = read8(m, LY);

				if (ly < 144) {
					draw_bg(ly, framebuffer, m);
					draw_fg(ly, framebuffer, m);
				}

				ly++;
				write8(m, LY, ly); // increase LY counter (scanline counter)
				
				if (ly == 144) {
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
				
				uint8_t ly = read8(m, LY) + 1;
				write8(m, LY, ly); // increase LY counter (scanline counter)
				
				if (ly == 154) {					
					draw_bg(ppu->scanline, framebuffer, m);
					draw_fg(ppu->scanline, framebuffer, m);
					ly = 0;
					write8(m, LY, ly); // reset LY
					ppu_mode_set(ppu,m,MODE_OAM);
				}
			} break;
			
		case MODE_OAM:
		
			if (ppu->dot >= 80) {
				ppu->dot -= 80;
				ppu_mode_set(ppu,m,MODE_VRAM);				
			} break;
		
		case MODE_VRAM:
		
			if (ppu->dot >= 166) {
				ppu->dot -= 166;
				uint8_t stat = read8(m, STAT);
				uint8_t lyc = read8(m, LYC);
				uint8_t ly = read8(m, LY);
				
				if (stat & (1 << 3)) {
					// interrupt
					uint8_t iflags = read8(m, IF);
					iflags |= (1 << 1); 
					write8(m, IF, iflags);
				}
				if ((lyc == ly) && (stat & (1<<6))) {
					// interrupt
					uint8_t iflags = read8(m, IF);
					iflags |= (1 << 1); 
					write8(m, IF, iflags);
				}
				
				stat &= ~(1<<2);
				stat |= ((lyc == ly) << 2);
				write8(m, STAT, stat);
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