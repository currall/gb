//ppu.h
#ifndef PPU_H
#define PPU_H

#include <stdint.h>
#include "memory.h"

#define MODE_HBLANK 0
#define MODE_VBLANK 1
#define MODE_OAM 	2
#define MODE_VRAM	3

// memory addresses of screen registers

#define LCDC 0xFF40
#define STAT 0xFF41
#define SCY  0xFF42
#define SCX  0xFF43
#define LY   0xFF44
#define LYC  0xFF45
#define BGP  0xFF47

typedef struct {
    int mode;		
    int scanline;	// number of scanlines
    int dot;		// cycles in current scanline
	int frame_ready; // is there a frame ready in the frame buffer?
} PPU;

void ppu_init(PPU* ppu, Memory* m);
void ppu_step(PPU* ppu, Memory* m);
int  ppu_frame_ready(PPU* ppu);
uint32_t* ppu_get_framebuffer(PPU* ppu);
void clear_framebuffer();

#endif