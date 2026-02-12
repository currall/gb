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

char* gb_init(char* file, Registers* reg, Memory* m, Status* s) {

	file = read_rom(file,m); // read into memory
	read_boot_ROM("dmg_boot.bin",&m->boot_rom); // read boot rom into boot rom memory

	cpu_init(reg);
	mem_init(m);
	status_init(s);

	return file;
}

int main(int argc, char *argv[]) {
	
	// create memory structures
	Registers reg; // cpu registers
	PPU ppu; // pixel processing unit (gpu)
	Memory m = {0}; // memory
	Status s = {0}; // debug emualtor status

	char* file = 0;
	if (argc>1) // read rom from arg if possible
        file = argv[1];
	
	// init system
	file = gb_init(file, &reg, &m, &s);
	
	// debug output
	if (s.print_memory) {print_memory(&m); s.print_memory=0;};
	if (s.print_cycle) {
		print_table_header(&s);
		print_cycle(&reg,&m,&s);
	}
	
	// create window
	ppu_init(&ppu, &m);
	window_init(file); // start main window
	vram_window_init(); // start vram window in background
	
	// cpu loop
	clock_t frame_start = clock();
	if (PRINT_DEBUG) printf("[MAIN] cpu cycle start\n");
	
	while(s.running){ // main loop
		
		// cpu cycles
		int c = cpu_step(&m,&reg);
		inc_cycle_counts(&s,c);
		
		// debug prints
		if (s.print_cycle) {print_cycle(&reg,&m,&s); check_events(&s,&m);} // check events during per-instruction debug so it can turn off
		if (s.print_memory) {print_memory(&m); s.print_memory=0;};
		
		for (int i = 0; i < c; i++) {
			dma_step(&m);
			ppu_step(&ppu,&m);
			timer_step(&m);
			if (ppu_frame_ready(&ppu)) {
				window_update(ppu_get_framebuffer(&ppu));
				ppu.frame_ready = 0;
			}
		}
		
		// pause
		if (s.paused > 0) {
			if (s.paused == 1) { // only print header if this is a new pause
				printf("\n[MAIN] CPU Paused!\n");
				print_table_header(&s);
			}
			print_cycle(&reg,&m,&s);
			pause_framebuffer(ppu_get_framebuffer(&ppu));
			window_update(ppu_get_framebuffer(&ppu));
			while(s.paused) {
				check_events(&s,&m);
				if (s.print_memory) {print_memory(&m); s.print_memory=0;};
				if (s.show_vram_viewer) vram_window_update(&m); else vram_window_hide(); // vram window
				if (s.advance_frame) {s.paused=0;} // frame stepping
			}
			if (s.advance_cycle) {s.paused=2;s.advance_cycle=0;} // paused = 2 to skip cycle header
		}
		
		// end timer
		
		if (s.frame_tracker > (CPU_HZ / 60)) { // code to run per-frame

			// input
			check_events(&s,&m);

			// restart
			if (s.restart_triggered) {
				s.restart_triggered = 0;
				gb_init(file, &reg, &m, &s);
			}
			if (s.new_game) {
				s.new_game = 0;
				gb_init(0, &reg, &m, &s);
			}

			// debug
			if (s.print_frame) print_cycle(&reg,&m,&s); // print cycle
			if (s.print_memory) {print_memory(&m); s.print_memory=0;};
			if (s.show_vram_viewer) vram_window_update(&m); else vram_window_hide(); // vram window
			if (s.advance_frame) {s.paused=2;s.advance_frame=0;} // frame stepping

			// === ADD ADDITIONAL PER-FRAME DEBUG CODE HERE ===

			// =====================================
			
			if (!s.fast_forward) { // skip frame delay if in fast forward
				while (((double)(clock() - frame_start) / CLOCKS_PER_SEC) < (1.0 / 60.0)) {
					// do nothing until frame time has passed
				}
			}
			// reset frame cycle counter
			s.frame_tracker = 0;
			frame_start = clock();
			
			// check if 1 second has passed
			if (s.second_tracker > CPU_HZ){
				s.total_seconds++;
				if (PRINT_DEBUG) printf("[TIME] %d seconds passed\n", s.total_seconds);
				s.second_tracker = 0;
			}
		}
	}
	
	window_destroy();
	vram_window_destroy();
    return 0;
}