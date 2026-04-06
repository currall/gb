// file.h
#ifndef FILE_H
#define FILE_H

#include <stdint.h>

typedef struct Memory Memory;
typedef struct Registers Registers;

uint32_t load_file(char* file, uint8_t** m, int warn);

// game save functions

char* savefile_name(char *file);
void save_game(char* file, uint8_t* eram, uint32_t ram_size);
void load_game(char* file, uint8_t* eram, uint32_t ram_size);

void save_state(char* f, Memory* m, Registers* reg);
void load_state(char* f, Memory* m, Registers* reg);

#endif