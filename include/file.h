// file.h
#ifndef FILE_H
#define FILE_H

#include <stdint.h>

int read_file(char* file, void* buf, int pos, int bytesToRead);
int load_rom(char* file, uint8_t** m);

#endif