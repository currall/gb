#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "file.h"
#include "header.h"
#include "memory.h"
#include "rom.h"

// registers
uint16_t AF;
uint16_t BC;
uint16_t DE;
uint16_t HL;
uint16_t SP;
uint16_t PC;

int main(int argc, char *argv[]){

    char* file;
    if (argc>1) // read rom from arg
        file = argv[1];
    else
        file = "test.gb";
    
    uint8_t memory[64*1024];

    Header h = readHeader(file);
    if (PRINT_ROM_HEADER == TRUE) printHeader(h);
    readROM(file,h,memory);

    return 0;
}