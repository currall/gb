#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "config.h"
#include "cpu.h"
#include "file.h"
#include "header.h"
#include "memory.h"
#include "ppu.h"
#include "registers.h"
#include "rom.h"
#include "window.h"

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

int main(int argc, char *argv[]) {
	
	// create memory structures
	Registers reg; // cpu registers
	PPU ppu; // pixel processing unit (gpu)
	Memory m = {0}; // memory
	
	// legacy memory declarations
	uint8_t boot_rom[0x100] = {0};
    uint8_t memory[0x10000] = {0};
	
	/*
	
	======================
	nintendo logo scrolled a bit before updating main.c and ppu.c to Memory struct
	something is dont to memory to stop it working outside of there
	
	*/

	// read ROM
    char* file;
    if (argc>1) // read rom from arg
        file = argv[1];
    else
        file = "test.gb";
	
	if (PRINT_DEBUG) printf("[MAIN] loading %s\n", file);
    Header h = read_header(file);
    read_ROM(file,h,m.memory); // read into memory
    read_boot_ROM(file,h,m.boot_rom); // read boot rom into separate memory
	
	// clear registers
	cpu_init(&reg);
	mem_init(&m);
	printf("PC=%04X LCDC=%02X FF50=%02X\n", reg.PC, m.memory[LCDC], m.memory[0xFF50]);
	
	// cycle tracking
	int cycles = 0; // tracks physical cpu cycles in ticks
	int instruction_count = 0; // tracks instructions run
	
	int second_tracker = 0; // counts cycles passed until 1 second
	int frame_tracker = 0; // counts cycles passed until 1 frame
	
	// debug output
    if (PRINT_ROM_HEADER == TRUE) print_header(h);
	if (PRINT_MEMORY == TRUE) print_ROM(m.memory);
	if (PRINT_CYCLE) {
		print_table_header();
		if (SHOW_INSTRUCTION) printf("%5d: 0x%02x | ",-1, m.memory[reg.PC]);
		if (SHOW_OPERANDS) printf("0x%02x | 0x%04x | ", m.memory[reg.PC+1],m.memory[reg.PC + 1] + (m.memory[reg.PC + 2] << 8));
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
	
	// create window
	ppu_init(&ppu, &m);
	if (!window_init()) { // window creation failed
        return 1;
    }
	
	// cpu loop
	int running = 1;
	if (PRINT_DEBUG) printf("[MAIN] cpu cycle start\n");
	
	//for (int i=0;i<50000;i++){ // debug first 10 instructions
	while(running){ // main loop
	
		// measure time taken to run instructions
		clock_t frame_start = clock();
		
		// process all instructions for frame
		while(frame_tracker < (CPU_HZ / 60)) {
			// cpu cycles
			int cycle_inc = cycle(&m,&reg);
			
			if (reg.PC == 0x0100) {
				printf("[CPU] Reached cartridge entry point (0x0100)\n");
			}
			
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
			
			// graphics (ppu)
			ppu_step(&ppu, cycle_inc,&m);
		}
		
		// update display			
		window_update(ppu_get_framebuffer(&ppu), &running);
		printf("PC=%04X LCDC=%02X FF50=%02X\n", reg.PC, m.memory[LCDC], m.memory[0xFF50]);
		//clear_framebuffer();
		
		// end timer
		while ((double)(clock() - frame_start) / CLOCKS_PER_SEC < FRAME_TIME) {
            // do nothing until frame time has passed
        }
		
		// reset frame cycle counter
		frame_tracker = 0;
		
		// check if 1 second has passed
		if (second_tracker > CPU_HZ){
			if (PRINT_TIME) printf("[TIME] 1 second passed\n");
			second_tracker = 0;
		}
	}
	
	window_destroy();
    return 0;
}