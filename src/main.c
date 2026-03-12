// standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// win32 libraries for reattaching to launch console
#ifdef _WIN32 
#include <windows.h>
#include <stdio.h>
#endif

// emulator libraries
#include "audio.h"
#include "cpu.h"
#include "debug.h"
#include "dma.h"
#include "file.h"
#include "header.h"
#include "memory.h"
#include "palette.h"
#include "ppu.h"
#include "registers.h"
#include "rom.h"
#include "timer.h"
#include "vram_window.h"
#include "window.h"

#define CPU_HZ 	4194304
#define FPS		60

void simulate_boot_rom(Registers* reg, Memory* m, uint8_t game_id) {
	reg->PC = 0x100;
	mem_boot(m);
	if (!m->cgb_mode) palette_init(m->ppu, game_id);
}

char* gb_init(char* file, Registers* reg, Memory* m, Status* s, Audio* audio) {	

	m->boot_rom_type = read_boot_ROM(&m->boot_rom); // read boot rom into boot rom memory
	file = read_rom(file,m,s); // read into memory
	
	cpu_init(reg);
	mem_init(m);
	status_init(s);
	audio_init(audio, m);
	
	switch (m->boot_rom_type) {
		case 0: simulate_boot_rom(reg,m, s->game_id); break; // simulate boot rom if not found
		case 1: palette_init(m->ppu, s->game_id); break; // dmg boot rom
	}
	printf("boot rom type %d\n", m->boot_rom_type);

	if (m->ram_size > 0) load_game(file,m->eram,m->ram_size); // attempt to load save file

	return file;
}

int main(int argc, char *argv[]) {

	#ifdef _WIN32
    // on windows, attach to cmd window that launched program
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
        freopen("CONIN$", "r", stdin);
    }
	#endif
	
	// create memory structures
	Registers reg 	= {0}; // cpu registers
	Memory m 		= {0}; // memory
	PPU ppu 		= {0}; // pixel processing unit (gpu)
	Status s 		= {0}; // debug emualtor status
	Audio audio 	= {0}; // audio

	m.ppu = &ppu;

	char* file = 0;
	if (argc>1) // read rom from arg if possible
        file = argv[1];
	
	// init system
	file = gb_init(file, &reg, &m, &s, &audio);
	
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
	float frame_start = clock();
	clock_t second_count = clock();
	if (PRINT_DEBUG) printf("[MAIN] cpu cycle start\n");

	
	while(s.running){ // main loop
		
		// cpu cycles
		int c = cpu_step(&m,&reg);
		inc_cycle_counts(&s,c);
		
		// debug prints
		if (s.print_cycle) {print_cycle(&reg,&m,&s); check_events(&s,&m);} // check events during per-instruction debug so it can turn off
		if (s.print_memory) {print_memory(&m); s.print_memory=0;};
		
		dma_step(&m, c);
		ppu_step(&ppu,&m, &s, c);
		timer_step(&m, c);
		audio_step(&audio, &m, c);

		if (ppu_frame_ready(&ppu)) {
			window_update(ppu_get_framebuffer(&ppu));
			ppu.frame_ready = 0;
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
				if (m.ram_size > 0) save_game(file, m.eram, m.ram_size); // save before resetting	
				file = gb_init(file, &reg, &m, &s, &audio);
				frame_start = clock(); // reset frame timer
			}
			if (s.new_game) {
				s.new_game = 0;
				if (m.ram_size > 0) save_game(file, m.eram, m.ram_size); // save before new game
				file = gb_init(0, &reg, &m, &s, &audio);
				frame_start = clock(); // reset frame timer
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
				frame_start += (float)CLOCKS_PER_SEC / 60;
			} else {
				frame_start = clock();
			}
			// reset frame cycle counter
			s.frame_tracker = 0;
		}

		// check if 1 second has passed
		if (s.second_tracker > (CPU_HZ)){ // code to run per emulated second

			uint64_t new_clock = clock();
			uint64_t ticks = new_clock - second_count;
			double ms = (double)ticks * 1000.0 / CLOCKS_PER_SEC;
			double percentage = (1000.0 / ms) * 100.0;
			double fps = 60 * (1000.0 / ms);
			second_count = new_clock;
			s.total_seconds++;

			if (PRINT_DEBUG)
				printf("[TIME] %d seconds passed [took %.0fms: %.2f%%] [FPS: %.2f]\n",
					s.total_seconds, ms, percentage, fps);
			
			s.second_tracker = 0;
		}
	}

	// if game has external ram, save it to save file
	if (m.ram_size > 0) save_game(file, m.eram, m.ram_size);
	
	window_destroy();
	vram_window_destroy();
    return 0;
}