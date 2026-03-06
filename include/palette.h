#ifndef PALETTE_H
#define PALETTE_H

#include <stdint.h>

typedef struct PPU PPU;

typedef struct Palette {
    uint32_t bg[4];
    uint32_t obj0[4];
    uint32_t obj1[4];
} Palette;

// game ids
typedef enum {
    ID_ALLEYWAY     = 0x88,
    ID_DR_MARIO     = 0x3C,
    ID_MARIO_LAND   = 0x46,
    ID_MARIO_LAND_2 = 0xC9,
    ID_POKEMONRED   = 0x14,
    ID_QIX          = 0xF2,
    ID_TETRIS       = 0xDB,
    ID_WARIO_LAND   = 0x59,
    ID_WARIO_LAND2  = 0xD3,
    ID_ZELDA        = 0x70,
} GameID;

// format conversion
uint32_t gbc_to_rgb(uint16_t color16);
uint16_t rgb_to_gbc(uint32_t rgb);

// palettes
Palette get_palette(uint8_t id, int custom);
void apply_palette(PPU* ppu, uint8_t id, int custom);

// palette setup
void palette_init(PPU* ppu, uint8_t id);

#endif