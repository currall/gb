//window.h
#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>

#include "memory.h"
#include "registers.h"

#define PRINT_DEBUG	1

typedef struct {
	
	// emulator status
	int running;
	int paused;

	int restart_triggered;
	int new_game;

	uint64_t cycles; // tracks physical cpu cycles in ticks
	uint64_t instruction_count; // tracks instructions run
	
	uint64_t div_tracker; // counts cycles in div
	uint64_t frame_tracker; // counts cycles passed until 1 frame
	uint64_t second_tracker; // counts cycles passed until 1 second
	uint64_t total_cycles;
	uint64_t total_seconds;

	// controls debug prints
	int print_cycle; // print debug info every instruction
	int print_frame; // print debug info every frame
	int print_memory;// print memory next cycle
	
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
	
	// emulation speed
	int advance_frame; // jump to next frame
	int advance_cycle; // jump to next instruction
	int fast_forward;
	
} Status;

void status_init(Status* s);

void print_table_header(Status* s);
void print_cycle(Registers* reg, Memory* m, Status* s);
void print_memory(Memory* m);

#endif