#include <SDL2/SDL.h>

#include "debug.h"
#include "memory.h"
#include "palette.h"
#include "ui.h"
#include "window.h"

uint32_t ui_framebuffer[160*144];

// fonts
uint8_t font[26][5] = {
    {0b11111, 0b10001, 0b11111, 0b10001, 0b10001}, // A
    {0b11110, 0b10001, 0b11110, 0b10001, 0b11110}, // B
    {0b11111, 0b10000, 0b10000, 0b10000, 0b11111}, // C
    {0b11110, 0b10001, 0b10001, 0b10001, 0b11110}, // D
    {0b11111, 0b10000, 0b11111, 0b10000, 0b11111}, // E
    {0b11111, 0b10000, 0b11111, 0b10000, 0b10000}, // F
    {0b11111, 0b10000, 0b10111, 0b10001, 0b11111}, // G
    {0b10001, 0b10001, 0b11111, 0b10001, 0b10001}, // H
    {0b11111, 0b00100, 0b00100, 0b00100, 0b11111}, // I
    {0b11111, 0b00010, 0b00010, 0b10010, 0b01100}, // J
    {0b10001, 0b10010, 0b11100, 0b10010, 0b10001}, // K
    {0b10000, 0b10000, 0b10000, 0b10000, 0b11111}, // L
    {0b11111, 0b10101, 0b10101, 0b10001, 0b10001}, // M
    {0b10001, 0b11001, 0b10101, 0b10011, 0b10001}, // N
    {0b01110, 0b10001, 0b10001, 0b10001, 0b01110}, // O
    {0b11110, 0b10001, 0b11110, 0b10000, 0b10000}, // P
    {0b01110, 0b10001, 0b10001, 0b10010, 0b01101}, // Q
    {0b11110, 0b10001, 0b11110, 0b10010, 0b10001}, // R
    {0b11111, 0b10000, 0b11111, 0b00001, 0b11111}, // S
    {0b11111, 0b00100, 0b00100, 0b00100, 0b00100}, // T
    {0b10001, 0b10001, 0b10001, 0b10001, 0b01110}, // U
    {0b10001, 0b10001, 0b10001, 0b01010, 0b00100}, // V
    {0b10001, 0b10001, 0b10101, 0b10101, 0b11011}, // W
    {0b10001, 0b01010, 0b00100, 0b01010, 0b10001}, // X
    {0b10001, 0b01010, 0b00100, 0b00100, 0b00100}, // Y
    {0b11111, 0b00010, 0b00100, 0b01000, 0b11111}, // Z
};

uint8_t numbers[10][5] = {
    {0b01110, 0b10001, 0b10001, 0b10001, 0b01110}, // 0
    {0b01100, 0b10100, 0b00100, 0b00100, 0b11111}, // 1
    {0b01110, 0b10001, 0b00110, 0b01000, 0b11111}, // 2
    {0b01110, 0b10001, 0b00110, 0b10001, 0b01110}, // 3
    {0b00010, 0b00110, 0b01010, 0b11111, 0b00010}, // 4
    {0b11111, 0b10000, 0b11110, 0b00001, 0b11110}, // 5
    {0b00100, 0b01000, 0b11110, 0b10001, 0b01110}, // 6
    {0b11111, 0b00010, 0b00100, 0b01000, 0b10000}, // 7
    {0b01110, 0b10001, 0b01110, 0b10001, 0b01110}, // 8
    {0b01110, 0b10001, 0b01110, 0b00010, 0b00100}, // 9
};

uint8_t symbols[6][5] = {
    {0b00000, 0b10000, 0b00000, 0b00000, 0b10000}, // :
    {0b00100, 0b01110, 0b10101, 0b00100, 0b00100}, // ^: up arrow (shift)
    {0b00000, 0b00000, 0b11110, 0b00000, 0b00000}, // -
    {0b01010, 0b11111, 0b01010, 0b11111, 0b01010}, // #
    {0b00001, 0b00001, 0b01001, 0b11111, 0b01000}, // }: return arrow (enter)
    {0b00100, 0b00100, 0b11111, 0b00100, 0b00100}, // +
};

// framebuffer functions
uint32_t* ui_get_framebuffer() {
    return ui_framebuffer;
}

void clear_ui_framebuffer(){
	for (int i = 0; i < (160*144); i++){
		ui_framebuffer[i] = 0;
	}
}

// basic draw functions
int draw_pixel(int x, int y, uint32_t color) {
    
    if (x < 0 || x > GB_WIDTH) return 1;
    if (y < 0 || y > GB_HEIGHT) return 1;

    ui_framebuffer[(y * GB_WIDTH) + x] = color;
    return 0;
}

void draw_rect(int start_x, int start_y, int end_x, int end_y, uint32_t color) {

    for (int row = start_y; row < end_y; row++) {
        for (int column = start_x; column < end_x; column++) {
            draw_pixel(column, row, color);
        }
    }

}
void draw_bg_rect(int x, int y, uint32_t color) {

    int start_x = x - 2;
    int start_y = y - 2;
    int end_x = x + 7;
    int end_y = y + 7;

    for (int row = start_y; row < end_y; row++) {
        for (int column = start_x; column < end_x; column++) {
            int index = (row * GB_WIDTH) + column;
            if (ui_framebuffer[index] == 0) draw_pixel(column, row, color);
        }
    }

}

// draw char functions
void draw_letter(char c, char base, int x, int y, uint32_t color) {

    uint8_t* letter = font[c - base];

    for (int row = 0; row < 5; row++) {
        for (int column = 0; column < 5; column++) {
            if (letter[row] & (1 << (4 - column))) {
                draw_pixel(x + column, y + row, color);
            }
        }
    }
}
void draw_number(char c, int x, int y, uint32_t color) {

    uint8_t* letter = numbers[c - '0'];

    for (int row = 0; row < 5; row++) {
        for (int column = 0; column < 5; column++) {
            if (letter[row] & (1 << (4 - column))) {
                draw_pixel(x + column, y + row, color);
            }
        }
    }
}
void draw_symbol(char c, int x, int y, uint32_t color) {

    int index = 0;
    switch(c) {
        case ':': index = 0; break;
        case '^': index = 1; break;
        case '-': index = 2; break;
        case '#': index = 3; break;
        case '}': index = 4; break;
        case '+': index = 5; break;
    }

    uint8_t* letter = symbols[index];

    for (int row = 0; row < 5; row++) {
        for (int column = 0; column < 5; column++) {
            if (letter[row] & (1 << (4 - column))) {
                draw_pixel(x + column, y + row, color);
            }
        }
    }
}

// print word / number functions
int print_word(char* string, int x, int y, uint32_t color, uint32_t bg) {

    int i = 0; // tracks pos in word

    while (string[i] != 0) {

        draw_bg_rect(x, y, bg);

        if (string[i] == ' ') ;
        else if (string[i] >= 'A' && string[i] <= 'Z') draw_letter(string[i], 'A', x, y, color);
        else if (string[i] >= 'a' && string[i] <= 'z') draw_letter(string[i], 'a', x, y, color);
        else if (string[i] >= '0' && string[i] <= '9') draw_number(string[i], x, y, color);
        else draw_symbol(string[i], x, y, color);
        i++;
        x += 7;
    }
    return x;
}

int print_number(int number, int x, int y, uint32_t color, uint32_t bg) {
    int length = 0;
    int temp = number;

    // count digits
    while (temp != 0) {
        temp /= 10;
        length++;
    }

    // 10^(length-1)
    int divisor = 1;
    for (int i = 1; i < length; i++) divisor *= 10;

    temp = number;
    while (divisor > 0) {        
        draw_bg_rect(x, y, bg);

        int digit = temp / divisor; // current leftmost digit
        draw_number(digit + '0',x,y,color);
        temp %= divisor; // remove leftmost digit
        divisor /= 10;   // move to next digit
        x += 7;
    }
    return x;
}
int print_combo(char* string, int number, int x, int y, uint32_t color, uint32_t bg) {
    x = print_word(string,x,y, color, bg);
    x = print_number(number,x,y, color, bg);
    return x;
}

// more ui

void draw_palette(Palette p, int x, int y) {

    for (int i = 0; i < 4; i++) {
        draw_rect(x,y,x+7,y+7,0xFF000000);
        draw_rect(x+1,y+1,x+6,y+6,p.bg[i]);
        x+=9;
    }
    x+=5;
    for (int i = 0; i < 4; i++) {
        draw_rect(x,y,x+7,y+7,0xFF000000);
        draw_rect(x+1,y+1,x+6,y+6,p.obj0[i]);
        x+=9;
    }
    x+=5;
    for (int i = 0; i < 4; i++) {
        draw_rect(x,y,x+7,y+7,0xFF000000);
        draw_rect(x+1,y+1,x+6,y+6,p.obj1[i]);
        x+=9;
    }

}
void print_palettes(uint32_t color, uint32_t bg, uint8_t game_id) {
    int x = 4;
    int y = 4;
    uint32_t no_bg = 0x00000000;
    draw_rect(x-2,y-2,x+135,y+121,bg); // draw window bg

    print_word("PALETTES",x,y,color,no_bg);
    y+=11;

    print_word("1:",x,y,color,no_bg); draw_palette(get_palette(0, 1), x+14, y); y+= 11;
    print_word("2:",x,y,color,no_bg); draw_palette(get_palette(game_id, 0), x+14, y); y+= 11;
    print_word("3:",x,y,color,no_bg); draw_palette(get_palette(1, 1), x+14, y); y+= 11;
    print_word("4:",x,y,color,no_bg); draw_palette(get_palette(2, 1), x+14, y); y+= 11;
    print_word("5:",x,y,color,no_bg); draw_palette(get_palette(ID_TETRIS, 0), x+14, y); y+= 11;
    print_word("6:",x,y,color,no_bg); draw_palette(get_palette(ID_ALLEYWAY, 0), x+14, y); y+= 11;
    print_word("7:",x,y,color,no_bg); draw_palette(get_palette(ID_MARIO_LAND_2, 0), x+14, y); y+= 11;
    print_word("8:",x,y,color,no_bg); draw_palette(get_palette(ID_WARIO_LAND, 0), x+14, y); y+= 11;
    print_word("9:",x,y,color,no_bg); draw_palette(get_palette(ID_DR_MARIO, 0), x+14, y); y+= 11;
    print_word("0:",x,y,color,no_bg); draw_palette(get_palette(ID_POKEMONRED, 0), x+14, y); y+= 11;

}

void print_controls(uint32_t color, uint32_t bg) {
    int x = 4;
    int y = 4;
    
    uint32_t no_bg = 0x00000000;
    draw_rect(x-2,y-2,(GB_WIDTH-2),(GB_HEIGHT-2),bg); // draw window bg

    print_word("CONTROLS",x,y,color,no_bg); y+=7;
    print_word("--------",x,y,color,no_bg); y+=7;

    print_word("FILE:",x,y,color,no_bg); y+=7;
    print_word("-CTRL O:OPEN NEW GAME",x,y,color,no_bg); y+=7;
    print_word("-CTRL R:RESET",x,y,color,no_bg); y+=7;
    print_word("-CTRL S:SAVE GAME",x,y,color,no_bg); y+=7;
    y+=4;
    print_word("VIDEO:",x,y,color,no_bg); y+=7;
    print_word("-F11   :FULLSCREEN",x,y,color,no_bg); y+=7;
    print_word("-ALT  }:FULLSCREEN",x,y,color,no_bg); y+=7;
    print_word("-CTRL #:WINDOW SCALE",x,y,color,no_bg); y+=7;
    print_word("-ALT   :COLOR PALETTE",x,y,color,no_bg); y+=7;
    print_word("-CTRL F:SHOW FPS",x,y,color,no_bg); y+=7;
    y+=4;
    print_word("SPEED:",x,y,color,no_bg); y+=7; 
    print_word("-CTRL P:PAUSE",x,y,color,no_bg); y+=7;
    print_word("-CTRL^F:FAST-FORWARD",x,y,color,no_bg); y+=7;
    print_word("-CTRL +:SPEED UP",x,y,color,no_bg); y+=7;
    print_word("-CTRL -:SPEED DOWN",x,y,color,no_bg); y+=7;
    print_word("-CTRL^+:RESET SPEED",x,y,color,no_bg); y+=7;

}

// main ui function
void update_ui(Status* s){

    uint32_t fg = 0xFF000000;
    uint32_t bg = 0xD0FFFFFF;

	clear_ui_framebuffer();
    int y = 4;

    if (s->ui_show_fps) {
        print_combo("FPS:",(int)s->fps,4,y,fg,bg);
        y += 7;
    }
	if (s->paused) {
        print_word("PAUSED",4,y,fg,bg);
        y += 7;
    }
    if (s->fast_forward) {
        print_word("FAST-FORWARD",4,y,fg,bg);
        y += 7;
    }
    if (s->speed != 100) {
        print_combo("SPEED:",s->speed,4,y,fg,bg);
        y += 7;
    }

    if (s->ui_show_palettes) print_palettes(fg,bg,s->game_id);
    if (s->ui_show_controls) print_controls(fg,bg);

}