#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "cpu.h"
#include "file.h"
#include "header.h"
#include "memory.h"
#include "registers.h"
#include "rom.h"

int main(int argc, char *argv[]){
	
	Registers reg;

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
	
	cycle(memory,reg);
	
	int cycles = 0;
	
	while(true){ // main loop
		cycles++;
	}

    return 0;
}