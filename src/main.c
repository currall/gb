#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "config.h"
#include "cpu.h"
#include "file.h"
#include "header.h"
#include "memory.h"
#include "registers.h"
#include "rom.h"

#define CPU_HZ 4194304

void print_table_header() {
	/*
	printf("\n");
	printf("==================================================================================================================\n");
	printf("instruction | 8bit | 16 bit | PC      | SP        | Register Values                               | total \n");
	printf("[no:opcode] | opnd | oprand | ProgCnt | StackPntr | AF        | BC        | DE        | HL        | cycles passed \n");
	printf("============|======|========|=========|===========|===========|===========|===========|===========|===============\n");
	*/
	
	char line1[128] = "";
	char line2[128] = "";
	char line3[128] = "";
	char line4[128] = "";
	
	if (SHOW_INSTRUCTION) {
		strcat(line1, "==============");
		strcat(line2, "instruction | ");
		strcat(line3, "[no:opcode] | ");
		strcat(line4, "============|=");
	}
	if (SHOW_OPERANDS) {
		strcat(line1, "================");
		strcat(line2, "8bit | 16 bit | ");
		strcat(line3, "opnd | oprand | ");
		strcat(line4, "==============|=");
	}
	if (SHOW_CPU) {
		strcat(line1, "======================");
		strcat(line2, "PC      | SP        | ");
		strcat(line3, "ProgCnt | StackPntr | ");
		strcat(line4, "========|===========|=");
	}
	if (SHOW_IME) {
		strcat(line1, "==================");
		strcat(line2, "Interrupts      | ");
		strcat(line3, "IME | IME_delay | ");
		strcat(line4, "====|===========|=");
	}
	if (SHOW_REGISTERS) {
		strcat(line1, "================================================");
		strcat(line2, "Register Values                               | ");
		strcat(line3, "AF        | BC        | DE        | HL        | ");
		strcat(line4, "==========|===========|===========|===========|=");
	}
	if (SHOW_CYCLES) {
		strcat(line1, "================");
		strcat(line2, "total         | ");
		strcat(line3, "cycles passed | ");
		strcat(line4, "==============|=");
	}
	
	printf("%s\n", line1);
    printf("%s\n", line2);
    printf("%s\n", line3);
    printf("%s\n", line4);
	
}

int main(int argc, char *argv[]){
	
	Registers reg;

    char* file;
    if (argc>1) // read rom from arg
        file = argv[1];
    else
        file = "test.gb";
    
    uint8_t memory[0x10000] = {0};

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
	reg.IME = 0;
	reg.IME_delay = 0;
	reg.halted = 0;
	
	int cycles = 0; // tracks physical cpu cycles in ticks
	int instruction_count = 0; // tracks instructions run
	
	int second_tracker = 0; // counts cycles passed until 1 second
	int frame_tracker = 0; // counts cycles passed until 1 frame
	
	if (PRINT_CYCLE) {
		print_table_header();
		if (SHOW_INSTRUCTION) printf("%5d: 0x%02x | ",-1, memory[reg.PC]);
		if (SHOW_OPERANDS) printf("0x%02x | 0x%04x | ", memory[reg.PC+1],memory[reg.PC + 1] + (memory[reg.PC + 2] << 8));
		if (SHOW_CPU) printf("PC:0x%02x | SP:0x%04x | ",reg.PC, reg.SP);
		if (SHOW_IME) printf("%-1d   | %-1d         | ",reg.IME, reg.IME_delay);
		if (SHOW_REGISTERS) {printf(
			"AF:0x%04x | BC:0x%04x | "
			"DE:0x%04x | HL:0x%04x | ",
			reg.AF,
			reg.BC,
			reg.DE,
			reg.HL
		);}
		if (SHOW_CYCLES) printf("%-13d | ",0);
		printf("\n");
		if (SHOW_INSTRUCTION) printf("%5d: ",0);
	}
	
	//for (int i=0;i<50000;i++){ // debug first 10 instructions
	while(true){ // main loop
		
		while(frame_tracker < (CPU_HZ / 60)) {
			int cycle_inc = cycle(memory,&reg);
			cycles += cycle_inc;
			second_tracker += cycle_inc;
			frame_tracker += cycle_inc;
			instruction_count++;
			
			// debug prints
			if (PRINT_CYCLE) {
				if (SHOW_CYCLES) printf("%-13d | ",cycles);
				printf("\n");
				if (SHOW_INSTRUCTION) printf("%5d: ",instruction_count);
			}
		}
		
		if (second_tracker>CPU_HZ){
			printf("1 second passed\n");
			second_tracker = 0;
		}
	}

    return 0;
}