#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "cpu.h"
#include "file.h"
#include "header.h"
#include "memory.h"
#include "rom.h"

// registers
uint8_t A;
uint8_t F;
uint8_t B;
uint8_t C;
uint8_t D;
uint8_t E;
uint8_t H;
uint8_t L;
uint16_t SP;
uint16_t PC = 0x0000;

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
	if (PRINT_MEMORY == TRUE) printROM(memory);
	
	cycle(memory,PC);
	
	while(true){ // main loop
	
		int x = 0; 
	
	}

    return 0;
}