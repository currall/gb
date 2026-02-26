
#include <stdio.h>
#include <stdint.h>

#include "ppu.h"

// convert 8bit color to gbc format

uint32_t gbc_to_rgb(uint16_t color16) {
    uint8_t r = (color16 & 0x1F);
    uint8_t g = (color16 >> 5) & 0x1F;
    uint8_t b = (color16 >> 10) & 0x1F;

    uint8_t R = (r << 3) | (r >> 2);
    uint8_t G = (g << 3) | (g >> 2);
    uint8_t B = (b << 3) | (b >> 2);

    return 0xFF000000 | (R << 16) | (G << 8) | B;
}

uint16_t rgb_to_gbc(uint32_t rgb)
{
    uint8_t R = (rgb >> 16) & 0xFF;
    uint8_t G = (rgb >> 8)  & 0xFF;
    uint8_t B =  rgb        & 0xFF;

    // Compress 8-bit → 5-bit
    uint8_t r = R >> 3;
    uint8_t g = G >> 3;
    uint8_t b = B >> 3;

    return (b << 10) | (g << 5) | r;
}

// color: 15 bit color 
// obj: set color of background or obj
// palette idx: index of palette to modify
// color idx: index of color within palette
void set_color(PPU* ppu, uint32_t c, int obj, uint8_t palette_idx, uint8_t color_idx) {

    uint8_t idx = (8 * palette_idx) + (color_idx * 2);
    uint16_t color = rgb_to_gbc(c);

    if (obj){
        ppu->obj_palette[idx] = color & 0xFF;        // lower 8 bits
        ppu->obj_palette[idx + 1] = (color >> 8) & 0x7F; // upper 7 bits
    } else {
        ppu->bg_palette[idx] = color & 0xFF;        // lower 8 bits
        ppu->bg_palette[idx + 1] = (color >> 8) & 0x7F; // upper 7 bits
    } 

}

void set_palette(PPU* ppu, int obj, uint8_t palette_idx, uint32_t* palette) {
    set_color(ppu, palette[0], obj, palette_idx, 0); // white
    set_color(ppu, palette[1], obj, palette_idx, 1); // light grey
    set_color(ppu, palette[2], obj, palette_idx, 2); // dark grey
    set_color(ppu, palette[3], obj, palette_idx, 3); // black
}

void new_palette(PPU* ppu){
    uint32_t palette[4] = {0xFFFFFFFF, 0xFFC0C0C0, 0xFF606060, 0xFF000000};
    set_palette(ppu,0,0,palette); // bg
    set_palette(ppu,1,0,palette); // obj0
    set_palette(ppu,1,1,palette); // obj1

}

void palette_init(PPU* ppu) {
    printf("[PALETTE] setting up palette!\n");
    new_palette(ppu);
}