#include <stdio.h>
#include <stdlib.h>

#include "file.h"
#include "header.h"
#include "memory.h"

// implement banking for larger roms here

void readROM(char* file, Header h, uint8_t* m){
    uint16_t ROMSizeB = 32 * 1024 * (1 << h.ROMSize); // rom size in bytes
    if (h.ROMSize == 0){
        readData(file, m, 0, ROMSizeB);
    }
    printf("%d",m);
}