#include <SDL2/SDL.h>

#include "debug.h"
#include "memory.h"
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
void draw_pixel(int x, int y, uint32_t color) {
    ui_framebuffer[(y * GB_WIDTH) + x] = color;
}

void draw_rect(int start_x, int start_y, int end_x, int end_y, uint32_t color) {

    for (int row = start_y; row < end_y; row++) {
        for (int column = start_x; column < end_x; column++) {
            draw_pixel(column, row, color);
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

    uint8_t* letter = font[c];

    for (int row = 0; row < 5; row++) {
        for (int column = 0; column < 5; column++) {
            if (letter[row] & (1 << (4 - column))) {
                draw_pixel(x + column, y + row, color);
            }
        }
    }
}

// print word / number functions
int print_word(char* string, int x, int y, uint32_t color) {

    int i = 0; // tracks pos in word
    int current_x = x; // tracks x pos of current letter

    while (string[i] != 0) {
        if (string[i] == ' ') ;
        else if (string[i] >= 'A' && string[i] <= 'Z') draw_letter(string[i], 'A', current_x, y, color);
        else if (string[i] >= 'a' && string[i] <= 'z') draw_letter(string[i], 'a', current_x, y, color);
        else if (string[i] >= '0' && string[i] <= '9') draw_number(string[i], current_x, y, color);
        else draw_symbol(string[i], current_x, y, color);
        i++;
        current_x += 7;
    }
    return current_x;
}

int print_number(int number, int x, int y, uint32_t color) {
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
    int current_x = x;
    while (divisor > 0) {
        int digit = temp / divisor; // current leftmost digit
        draw_number(
            digit + '0',
            current_x,y,color
        );
        temp %= divisor; // remove leftmost digit
        divisor /= 10;   // move to next digit
        current_x += 7;
    }
    return current_x;
}
int print_combo(char* string, int number, int x, int y, uint32_t color) {
    int current_x = x;
    current_x = print_word(string,current_x,y, 0xFF000000);
    current_x = print_number(number,current_x,y, 0xFF000000);
    return current_x;
}

// main ui function
void update_ui(Status* s){

	clear_ui_framebuffer();
	if (s->paused) {
        draw_rect(2,11,55,18,0xD0FFFFFF);
        print_word("PAUSED",4,11,0xFF000000);
    }
    draw_rect(2,2,55,11,0xD0FFFFFF);
    print_combo("FPS ",(int)s->fps,4,4,0xFF000000);

}