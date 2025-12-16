#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "config.h"
#include "cpu.h"
#include "file.h"
#include "header.h"
#include "memory.h"
#include "registers.h"
#include "rom.h"

void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void print_table_header() {
	
	printf("\n");
	printf("==================================================================================================================\n");
	printf("instruction | 8bit | 16 bit | PC      | SP        | Register Values                               | total \n");
	printf("[no:opcode] | opnd | oprand | ProgCnt | StackPntr | AF        | BC        | DE        | HL        | cycles passed \n");
	printf("============|======|========|=========|===========|===========|===========|===========|===========|===============\n");
	
}

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
	
	reg.BC = 0;
	reg.DE = 0;
	reg.HL = 0;
	reg.AF = 0;
	reg.PC = 0;
	reg.SP = 0xFFFE; // stack starts at top of memory
	
	int cycles = 0; // tracks physical cpu cycles in ticks
	int instruction_count = 0; // tracks instructions run
	
	print_table_header();
	//for (int i=0;i<100;i++){ // debug first 10 instructions
	while(true){ // main loop
		
		printf("%05d: ",instruction_count); // debug print intruction count 
		int cycle_inc = cycle(memory,&reg);
		cycles += cycle_inc;
		printf(" | %d \n",cycles); // debug print intruction count
		
		instruction_count++;
	}

    return 0;
}