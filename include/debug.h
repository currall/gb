//window.h
#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>

#include "memory.h"
#include "registers.h"

#define PRINT_DEBUG	1

typedef struct {
	
	int running;
	int paused;
	
	int print_cycle; // print debug info every instruction
	int print_frame; // print debug info every frame
	int print_memory;// print memory next cycle

	int advance_frame; // jump to next frame
	
	// define what information should be shown in debug outputs
	// on pause, logging mode, etc.
	int show_instruction;
	int show_operands;
	int show_cpu;
	int show_registers;
	int show_ppu;
	int show_mbc;
	int show_ime;
	int show_cycles;
	
	int show_vram_viewer;
	
	// speeds up/slows down emulation
	float frame_time;
	int fast_forward;
	
} Status;

void status_init(Status* s);

void print_table_header(Status* s);
void print_cycle(Registers* reg, Memory* m, int instruction, uint64_t cycles, Status* s);
void print_memory(Memory* m);

#endif