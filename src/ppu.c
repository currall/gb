#include <stdio.h>

#include "debug.h"
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
uint8_t priority_map[160]; // allows background to have priority over sprites. stores one scanline

uint32_t palette[4] = {0xFFFFFFFF, 0xFFC0C0C0, 0xFF606060, 0xFF000000}; // use only grey palette for gb for now

uint32_t gbc_to_rgb(uint16_t color16) {
    uint8_t r = (color16 & 0x1F);
    uint8_t g = (color16 >> 5) & 0x1F;
    uint8_t b = (color16 >> 10) & 0x1F;

    uint8_t R = (r << 3) | (r >> 2);
    uint8_t G = (g << 3) | (g >> 2);
    uint8_t B = (b << 3) | (b >> 2);

    return 0xFF000000 | (R << 16) | (G << 8) | B;
}

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

void draw_bg(PPU* ppu, uint32_t* framebuffer, Memory* m) {
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
	for (int i = 0; i < 160; i++) priority_map[i] = 0; // clear priority map

	uint8_t lcdc = read8(m, LCDC);
	int scanline = ppu->scanline;
	// white background using lcdc only for dmg
    if (!m->cgb_mode && !(lcdc & 0x01)) {
        for (int x = 0; x < 160; x++) {
            framebuffer[scanline * 160 + x] = palette[0]; 
            priority_map[x] = 0; // no priority over sprites
        }
        return;
    }
	
    uint8_t scx = read8(m, SCX);
	uint8_t scy = read8(m, SCY);
	uint8_t bgp = read8(m, 0xFF47); // background palatte register
	uint8_t wx  = read8(m, WX);
    uint8_t wy  = read8(m, WY);

	int window_enable = (lcdc & 0x20) && (scanline >= wy);
	uint16_t bg_map_base = (lcdc & 0x08) ? 0x9C00 : 0x9800;
    uint16_t win_map_base = (lcdc & 0x40) ? 0x9C00 : 0x9800;
    int is_signed = !(lcdc & 0x10); 

    for (int x = 0; x < 160; x++) {
        int window_x_trigger = wx - 7;
        if (window_x_trigger < 0) window_x_trigger = 0; // prevent wx - 7 being less than 0

        int using_window = window_enable && (x >= window_x_trigger);

		uint16_t tile_map;
        int tile_row_idx; // "y" inside the map
        int tile_col_idx; // "x" inside the map
        int pixel_y;      // line inside the tile (0-7)
        int pixel_x;      // column inside the tile (0-7)

        if (using_window) {
            tile_map = win_map_base;
            tile_row_idx = ppu->window_line / 8;
            tile_col_idx = (x - (wx - 7)) / 8; // no scrolling on window layer
            pixel_y = ppu->window_line % 8;
            pixel_x = (x - (wx - 7)) % 8;
        } else {
            tile_map = bg_map_base;
            tile_row_idx = ((scanline + scy) & 0xFF) / 8;
            tile_col_idx = ((x + scx) & 0xFF) / 8;
            pixel_y = (scanline + scy) % 8;
            pixel_x = (x + scx) % 8;
        }

        uint16_t tile_address = tile_map + (tile_row_idx * 32) + tile_col_idx;
        uint8_t tile_id = read_vram(m, tile_address, 0);
	
		// read tile attributes
		uint8_t attr = read_vram(m, tile_address, 1);
		int cgb_palette	   = attr & 0x07;
		int vram_bank      = (attr >> 3) & 1;
		int h_flip         = (attr >> 5) & 1;
		int v_flip         = (attr >> 6) & 1;

		// tile data
        uint16_t current_tile_addr;
        if (is_signed) {
			current_tile_addr = 0x9000 + ((int8_t)tile_id) * 16;
        } else {
			current_tile_addr = 0x8000 + tile_id * 16;
        }
		
		// handle flip
		if (v_flip) pixel_y = 7 - pixel_y;
		int bit;
		if (h_flip) bit = pixel_x;
		else bit = 7 - pixel_x;

        uint8_t low  = read_vram(m, current_tile_addr + pixel_y * 2, vram_bank);
        uint8_t high = read_vram(m, current_tile_addr + pixel_y * 2 + 1, vram_bank);

        int color_idx = ((high >> bit) & 1) << 1 | ((low >> bit) & 1);
        priority_map[x] = ((attr >> 7) & 0x01) << 7 | (color_idx & 0x03);
        
        if (m->cgb_mode) {
            int palette_addr = (cgb_palette * 8) + (color_idx * 2);
            uint16_t color16 = ppu->bg_palette[palette_addr] | (ppu->bg_palette[palette_addr + 1] << 8);
            framebuffer[scanline * 160 + x] = gbc_to_rgb(color16);
        } else {
            int shade = (bgp >> (color_idx * 2)) & 0x03;
            framebuffer[scanline * 160 + x] = palette[shade];
        }

    }
	if (window_enable) ppu->window_line++;
}

void draw_fg(PPU* ppu, uint32_t* framebuffer, Memory* m) {
	
	uint8_t lcdc = read8(m, LCDC);
    if (!(lcdc & 0x02)) return; // obj enable flag

    int sprite_height = (lcdc & 0x04) ? 16 : 8;
    int sprites_drawn = 0; // gameboy has limit of 10 sprites per scanline
	
	for (int i = 0; i < 40 && sprites_drawn < 10; i++) { // 40 sprites total, 10 per scanline
		uint16_t oam_addr = 0xFE00 + i * 4;
        
        int y = read8(m, oam_addr) - 16;
        int x = read8(m, oam_addr + 1) - 8;
        uint8_t tile_id = read8(m, oam_addr + 2);
		uint8_t attr = read8(m, oam_addr + 3);

        if (ppu->scanline < y || ppu->scanline >= y + sprite_height) continue; // skip if offscreen
		sprites_drawn++;

		// if (sprite_height == 16) tile_id &= 0xFE; // lsb of index is ignored for 16px tile

		int cgb_palette		= attr & 0x07;         // palette
        int vram_bank       = (attr >> 3) & 1;  // vram bank of tile

        int line = ppu->scanline - y; // line inside tile
        if (attr & 0x40) line = sprite_height - 1 - line; // vertical flip attribute
        if (sprite_height == 16) { // 16px tile handling for cgb
            tile_id &= 0xFE; 
            if (line >= 8) {
                tile_id |= 0x01;
                line -= 8;
            }
        }
		
		uint16_t tile_addr = 0x8000 + tile_id * 16 + line * 2;
        uint8_t low  = read_vram(m, tile_addr, vram_bank);
        uint8_t high = read_vram(m, tile_addr + 1, vram_bank);

        uint16_t palette_reg_addr = (attr & 0x10) ? 0xFF49 : 0xFF48;
        uint8_t palette_reg = read8(m, palette_reg_addr);
		
		for (int px = 0; px < 8; px++) { // loop throuhg pixels horizontally (8px wide)
			
            int screen_x = x + px;
            if (screen_x < 0 || screen_x >= 160) continue; // skip if offscreen
			
			// horizontal flip
			int bit = (attr & 0x20) ? px : (7 - px);
            int color_idx = ((high >> bit) & 1) << 1 | ((low >> bit) & 1);
			if (color_idx == 0) continue; // hide transparent pixels
			
			if (screen_x < 0 || screen_x >= 160) continue; // skip sprite if off-screen

            uint8_t bg_data = priority_map[screen_x];
            int bg_color = bg_data & 0x03;
            int bg_priority = (bg_data >> 7) & 1;
            int priority = (attr >> 7) & 1;
            int can_draw = 0;

            if (m->cgb_mode) {
                if (!(lcdc & 0x01)) can_draw = 1; // lcdc 0 - sprites over bg
                else if (bg_color == 0) can_draw = 1; // bg transparent
                else if (bg_priority == 0 && priority == 0) can_draw = 1;
            } else {
                if (priority == 0 || bg_color == 0) can_draw = 1;
            }
			
            if (can_draw) {
                if (m->cgb_mode){
                    int palette_addr = (cgb_palette * 8) + (color_idx * 2); 
                    uint16_t color16 = ppu->obj_palette[palette_addr] | (ppu->obj_palette[palette_addr + 1] << 8);
                    framebuffer[ppu->scanline * 160 + screen_x] = gbc_to_rgb(color16);
                } else {
                    int shade = (palette_reg >> (color_idx * 2)) & 0x03;
                    if (palette_reg_addr == 0xFF48) framebuffer[ppu->scanline * 160 + screen_x] = palette[shade];
                    if (palette_reg_addr == 0xFF49) framebuffer[ppu->scanline * 160 + screen_x] = palette[shade];
                }
            }
		}
	}
}

void ppu_step(PPU* ppu, Memory* m, Status* s, int cycles) {

	uint8_t lcdc = read8(m, LCDC);
    
    if (!(lcdc & 0x80)) { // lcd disabled
        ppu->scanline = 0;
		ppu->window_line = 0;
        ppu->dot = 0;
        ppu->mode = MODE_OAM;
		ppu->frame_ready = 0;
        write8(m, LY, 0); // set scanline to 0
        uint8_t stat = read8(m, STAT);
        write8(m, STAT, stat & 0xFC); 
        return;
    }
	
    ppu->dot += cycles;
	
	switch (ppu->mode) {
		
		case MODE_HBLANK:
		
			if (ppu->dot >= 204) { // end of hblank
				ppu->dot -= 204; // return to start of scanline

				ppu->scanline++;
				write8(m, LY, ppu->scanline); // store scanline in memory
				check_lyc(ppu,m);
				
				if (ppu->scanline == 144) {
					ppu_mode_set(ppu, m, MODE_VBLANK);
					ppu->frame_ready = 1;
					// vblank interrupt
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
					ppu->window_line = 0;
					write8(m, LY, ppu->scanline); // reset LY
					ppu_mode_set(ppu,m,MODE_OAM);
				}
			} break;
			
		case MODE_OAM:
		
			if (ppu->dot >= 80) {
				ppu->dot -= 80;

				if (ppu->scanline < 144) {
					draw_bg(ppu, framebuffer, m);
					draw_fg(ppu, framebuffer, m);
				}
				ppu_mode_set(ppu,m,MODE_VRAM);				
			} break;
		
		case MODE_VRAM:
		
			if (ppu->dot >= 172) {
				ppu->dot -= 172;
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