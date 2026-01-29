#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "cpu.h"
#include "debug.h"
#include "dma.h"
#include "file.h"
#include "header.h"
#include "memory.h"
#include "ppu.h"
#include "registers.h"
#include "rom.h"
#include "timer.h"
#include "vram_window.h"
#include "window.h"

#define CPU_HZ 		4194304
#define FPS			60

int main(int argc, char *argv[]) {
	
	// create memory structures
	Registers reg; // cpu registers
	PPU ppu; // pixel processing unit (gpu)
	Memory m = {0}; // memory
	Status s;
	
	// legacy memory declarations
	uint8_t boot_rom[0x100] = {0};
    uint8_t memory[0x10000] = {0};

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
	
    read_ROM(file,h,m.rom); // read rom into rom copy
	
	// clear registers
	cpu_init(&reg);
	mem_init(&m);
	status_init(&s);
	
	// cycle tracking
	int cycles = 0; // tracks physical cpu cycles in ticks
	int instruction_count = 0; // tracks instructions run
	
	int div_tracker = 0; // counts cycles in div
	int frame_tracker = 0; // counts cycles passed until 1 frame
	int second_tracker = 0; // counts cycles passed until 1 second
	uint64_t total_cycles = 0;
	
	// debug output
    print_header(h);
	if (s.print_memory) {print_memory(&m); s.print_memory=0;};
	if (s.print_cycle) {
		print_table_header(&s);
		print_cycle(&reg,&m,0,0,&s);
	}
	
	// create window
	ppu_init(&ppu, &m);
	if (!window_init(file)) { // window creation failed
        return 1;
    }
	vram_window_init();
	
	// cpu loop
	clock_t frame_start = clock();
	if (PRINT_DEBUG) printf("[MAIN] cpu cycle start\n");
	
	while(s.running){ // main loop
		// cpu cycles
		int cycle_inc = cpu_step(&m,&reg);
		
		cycles += cycle_inc;
		div_tracker += cycle_inc;
		frame_tracker += cycle_inc;
		second_tracker += cycle_inc;
		total_cycles += cycle_inc;
		instruction_count++;
		
		// debug prints
		if (s.print_cycle) print_cycle(&reg,&m,instruction_count,total_cycles,&s);
		if (s.print_memory) {print_memory(&m); s.print_memory=0;};
		
		for (int i = 0; i < cycle_inc; i++) {
			dma_step(&m);
			ppu_step(&ppu,&m);
			timer_step(&m);
			if (ppu_frame_ready(&ppu)) 
				window_update(ppu_get_framebuffer(&ppu));
				ppu.frame_ready = 0;
		}
		
		// graphics (ppu)
		//ppu_step(&ppu, cycle_inc,&m);
		
		// update display			
		
		
		check_events(&s);
		
		// pause
		if (s.paused) {
			printf("\n[MAIN] CPU Paused!\n");
			print_table_header(&s);
			print_cycle(&reg,&m,instruction_count,total_cycles,&s);
			pause_framebuffer(ppu_get_framebuffer(&ppu));
			window_update(ppu_get_framebuffer(&ppu));
			while(s.paused) {
				check_events(&s);
				if (s.print_memory) {print_memory(&m); s.print_memory=0;};
			}
		}
		
		// end timer
		
		if (frame_tracker > (CPU_HZ / 60)) {
			if (s.print_frame) print_cycle(&reg,&m,instruction_count,total_cycles,&s);
			
			if (s.show_vram_viewer) vram_window_update(&m);
			else vram_window_hide();
			
			while ((double)(clock() - frame_start) / CLOCKS_PER_SEC < s.frame_time) {
				// do nothing until frame time has passed
			}
			// reset frame cycle counter
			frame_tracker = 0;
			frame_start = clock();
			
			// check if 1 second has passed
			if (second_tracker > CPU_HZ){
				if (PRINT_DEBUG) printf("[TIME] 1 second passed\n");
				second_tracker = 0;
			}
		}
	}
	
	window_destroy();
	vram_window_destroy();
    return 0;
}