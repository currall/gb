// file.h
#ifndef FILE_H
#define FILE_H

#include <stdint.h>

int load_file(char* file, uint8_t** m);

// game save functions

char* savefile_name(char *file);
void save_game(char* file, uint8_t* eram, uint32_t ram_size);
void load_game(char* file, uint8_t* eram, uint32_t ram_size);

#endif