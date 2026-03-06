
#include <stdio.h>
#include <stdint.h>

#include "palette.h"
#include "ppu.h"

Palette create_palette(uint32_t* bg, uint32_t* obj0, uint32_t* obj1){
    Palette p = {0};
    for (int i = 0; i < 4; i++) { // for each color in palette
        p.bg[i] = bg[i];
        p.obj0[i] = obj0[i];
        p.obj1[i] = obj1[i];
    }
    return p;
}

Palette palette_bw(){
    uint32_t bg[4]   = {0xFFFFFFFF, 0xFFC0C0C0, 0xFF606060, 0xFF000000};
    uint32_t obj0[4] = {0xFFFFFFFF, 0xFFC0C0C0, 0xFF606060, 0xFF000000};
    uint32_t obj1[4] = {0xFFFFFFFF, 0xFFC0C0C0, 0xFF606060, 0xFF000000};
    return create_palette(bg, obj0, obj1);
}

Palette palette_dmg(){
    uint32_t bg[4]   = {0xFF9BBC0F, 0xFF8BAC0F, 0xFF306230, 0xFF0F380F};
    uint32_t obj0[4] = {0xFF9BBC0F, 0xFF8BAC0F, 0xFF306230, 0xFF0F380F};
    uint32_t obj1[4] = {0xFF9BBC0F, 0xFF8BAC0F, 0xFF306230, 0xFF0F380F};
    return create_palette(bg, obj0, obj1);
}

Palette palette_pink(){
    uint32_t bg[4]   = {0xffffe5ec, 0xFFffc2d1, 0xFFc9184a, 0xFF800f2f};
    uint32_t obj0[4] = {0xffffe5ec, 0xFFffc2d1, 0xFFc9184a, 0xFF800f2f};
    uint32_t obj1[4] = {0xffffe5ec, 0xFFffc2d1, 0xFFc9184a, 0xFF800f2f};
    return create_palette(bg, obj0, obj1);
}

Palette palette_tetris(){
    uint32_t bg[4]   = {0xFFFFFFFF, 0xFFFFFF00, 0xFFFF0000, 0xFF000000};
    uint32_t obj0[4] = {0xFFFFFFFF, 0xFFFFFF00, 0xFFFF0000, 0xFF000000};
    uint32_t obj1[4] = {0xFFFFFFFF, 0xFF5ABDFF, 0xFFFF0000, 0xFF0000FF};
    return create_palette(bg, obj0, obj1);
}

Palette palette_mario_land(){
    uint32_t bg[4]   = {0xFFB5B5FF, 0xFFFFFF94, 0xFFAD5A42, 0xFF000000};
    uint32_t obj0[4] = {0xFF000000, 0xFFFFFFFF, 0xFFFF8484, 0xFF943A3A};
    uint32_t obj1[4] = {0xFF000000, 0xFFFFFFFF, 0xFFFF8484, 0xFF943A3A};
    return create_palette(bg, obj0, obj1);
}

Palette palette_mario_land_2(){
    uint32_t bg[4]   = {0xFFFFFFCE, 0xFF63EFEF, 0xFF9C8431, 0xFF5A5A5A};
    uint32_t obj0[4] = {0xFFFFFFFF, 0xFFFF7300, 0xFF944200, 0xFF000000};
    uint32_t obj1[4] = {0xFFFFFFFF, 0xFF63A5FF, 0xFF0000FF, 0xFF000000};
    return create_palette(bg, obj0, obj1);
}

Palette palette_wario_land(){
    uint32_t bg[4]   = {0xFFFFFFFF, 0xFFADAD84, 0xFF42737B, 0xFF000000};
    uint32_t obj0[4] = {0xFFFFFFFF, 0xFFFF7300, 0xFF944200, 0xFF000000};
    uint32_t obj1[4] = {0xFFFFFFFF, 0xFF5ABDFF, 0xFFFF0000, 0xFF0000FF};
    return create_palette(bg, obj0, obj1);
}

Palette palette_wario_land_2(){
    uint32_t bg[4]   = {0xFFFFFFFF, 0xFFADAD84, 0xFF42737B, 0xFF000000};
    uint32_t obj0[4] = {0xFFFFFFFF, 0xFFFFAD63, 0xFF843100, 0xFF000000};
    uint32_t obj1[4] = {0xFFFFFFFF, 0xFF63A5FF, 0xFF0000FF, 0xFF000000};
    return create_palette(bg, obj0, obj1);
}

Palette palette_dr_mario(){
    uint32_t bg[4]   = {0xFFFFFFFF, 0xFF63A5FF, 0xFF0000FF, 0xFF000000};
    uint32_t obj0[4] = {0xFFFFFFFF, 0xFF63A5FF, 0xFF0000FF, 0xFF000000};
    uint32_t obj1[4] = {0xFFFFFFFF, 0xFFFF8484, 0xFF943A3A, 0xFF000000};
    return create_palette(bg, obj0, obj1);
}

Palette palette_zelda(){
    uint32_t bg[4]   = {0xFFFFFFFF, 0xFFFF8484, 0xFF943A3A, 0xFF000000};
    uint32_t obj0[4] = {0xFFFFFFFF, 0xFF00FF00, 0xFF318400, 0xFF004A00};
    uint32_t obj1[4] = {0xFFFFFFFF, 0xFF63A5FF, 0xFF0000FF, 0xFF000000};
    return create_palette(bg, obj0, obj1);
}

Palette palette_pokemon_red(){
    uint32_t bg[4]   = {0xFFFFFFFF, 0xFFFF8484, 0xFF943A3A, 0xFF000000};
    uint32_t obj0[4] = {0xFFFFFFFF, 0xFF7BFF31, 0xFF008400, 0xFF000000};
    uint32_t obj1[4] = {0xFFFFFFFF, 0xFFFF8484, 0xFF943A3A, 0xFF000000};
    return create_palette(bg, obj0, obj1);
}

Palette palette_alleyway(){
    uint32_t bg[4]   = {0xFFA59CFF, 0xFFFFFF00, 0xFF006300, 0xFF000000};
    uint32_t obj0[4] = {0xFFA59CFF, 0xFFFFFF00, 0xFF006300, 0xFF000000};
    uint32_t obj1[4] = {0xFFA59CFF, 0xFFFFFF00, 0xFF006300, 0xFF000000};
    return create_palette(bg, obj0, obj1);
}

Palette get_palette(uint8_t id, int custom) {
    if (custom) {
        switch (id){
            case 0: return palette_bw();
            case 1: return palette_dmg();
            case 2: return palette_pink();
        }
    } else {
        switch (id){
            case ID_ALLEYWAY: return palette_alleyway();
            case ID_DR_MARIO: return palette_dr_mario();
            case ID_MARIO_LAND: return palette_mario_land();
            case ID_MARIO_LAND_2: return palette_mario_land_2();
            case ID_POKEMONRED: return palette_pokemon_red();
            case ID_QIX: return palette_tetris();
            case ID_TETRIS: return palette_tetris();
            case ID_WARIO_LAND: return palette_wario_land();
            case ID_WARIO_LAND2: return palette_wario_land_2();
            case ID_ZELDA: return palette_zelda();
        }
    }

    return palette_bw();
}

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

uint16_t rgb_to_gbc(uint32_t rgb) {
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

void apply_palette(PPU* ppu, uint8_t id, int custom){
    Palette p = get_palette(id,custom);
    set_palette(ppu,0,0,p.bg); // bg
    set_palette(ppu,1,0,p.obj0); // obj0
    set_palette(ppu,1,1,p.obj1); // obj1
}

void palette_init(PPU* ppu, uint8_t id) {
    apply_palette(ppu,id,0);
}