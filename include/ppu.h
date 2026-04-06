//ppu.h
#ifndef PPU_H
#define PPU_H

#include <stdint.h>

typedef struct Memory Memory;
typedef struct Status Status;

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
#define WY   0xFF4A
#define WX   0xFF4B

// gbc registers
#define VRAM_BANK 0xFF4F
#define BCPS 0xFF68 // bg plette idx
#define BCPD 0xFF69 // bg palette data
#define OCPS 0xFF6A // obj palette idx
#define OCPD 0xFF6B // obj palette data

typedef struct PPU {

    uint32_t framebuffer[160*144]; // safe framebuffer to draw to screen

    uint16_t mode;		
    uint16_t scanline;	// number of scanlines
    uint16_t dot;		// cycles in current scanline
    uint8_t window_line; // line of window texture

    // --- cgb ---
    uint8_t bg_palette[64];
    uint8_t obj_palette[64];
} PPU;

void ppu_init(PPU* ppu, Memory* m);
void ppu_step(PPU* ppu, Memory* m, Status* s, int cycles);
int  ppu_frame_ready(PPU* ppu);
uint32_t* ppu_get_framebuffer(PPU* ppu);
void clear_framebuffer();

#endif