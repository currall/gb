#ifndef PALETTE_H
#define PALETTE_H

typedef struct PPU PPU;

// format conversion
uint32_t gbc_to_rgb(uint16_t color16);
uint32_t rgb_to_gbc(uint16_t color16);

// palette setup
void palette_init(PPU* ppu);

#endif