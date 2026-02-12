#include "debug.h"
#include "window.h"

#include <SDL2/SDL.h>

static SDL_Window*   window   = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture*  texture  = NULL;
uint32_t window_id = 0;

int window_init(char* file) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        return 0;
	
	char title[256];
	snprintf(title, sizeof(title), "gb - %s", file);

    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        GB_WIDTH * 4,
        GB_HEIGHT * 4,
        SDL_WINDOW_SHOWN
    );

    if (!window) return 0;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture  = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        GB_WIDTH,
        GB_HEIGHT
    );
	
	window_id = SDL_GetWindowID(window);
	SDL_RaiseWindow(window);
	SDL_SetWindowInputFocus(window);

    return renderer && texture;
}

void window_update(uint32_t* framebuffer) {
    SDL_UpdateTexture(texture, NULL, framebuffer, GB_WIDTH * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void joypad_set_button(Memory* m, uint8_t mask, int pressed, int is_button_group)
{
    uint8_t* group; // use buttons or dpad
	if (is_button_group) group = &m->j.buttons;
	else group = &m->j.dpad;

    uint8_t before = *group;

    if (pressed)
        *group &= ~mask;
    else
        *group |= mask;

    uint8_t after = *group;

    if ((before & ~after) != 0) {
		printf("[INPUT] Joypad input detected!\n");
		uint8_t iflags = read8(m, 0xFF0F);
		write8(m, 0xFF0F, iflags | 0x10);
    }
}

void check_events(Status* s, Memory* m){	

	/*
	=== KEYBINDS ===
	
	CONTROLS
	w/up: up dpad
	a/left: left dpad
	s/down: down dpad
	d/right: right dpad
	,/x: b
	./z: a
	enter: start
	backspace: select
	
	EMULATION
	n: advance to next frame
	p: toggle pause
	v: toggle vram viewer
	
	LOGGING
	l: enable logging every frame
	m: print memory to console
	right shift: logging every instruction (VERY SLOW)
	
	SPEED
	f: fast-forward (run as fast as possible, ignoring per-frame delay)
	-: slow down emulation
	+: speed up emulation
	
	*/

	uint8_t input = 0x3F;

	SDL_Event e;

	while (SDL_PollEvent(&e)) {

		SDL_Keycode key = e.key.keysym.sym;
		SDL_Keymod mods = SDL_GetModState();
		
		if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) {
			window_destroy();
			s->paused = 0;
			s->running = 0;
		}
		if (e.type == SDL_KEYDOWN && !e.key.repeat) {
			// controls
			if (key == SDLK_w || key == SDLK_UP) joypad_set_button(m, JP_UP, 1, 0);
			if (key == SDLK_a || key == SDLK_LEFT) joypad_set_button(m, JP_LEFT, 1, 0);
			if (key == SDLK_s || key == SDLK_DOWN) joypad_set_button(m, JP_DOWN, 1, 0);
			if (key == SDLK_d || key == SDLK_RIGHT) joypad_set_button(m, JP_RIGHT, 1, 0);

			if (key == SDLK_PERIOD || key == SDLK_z) joypad_set_button(m, JP_A, 1, 1);
			if (key == SDLK_COMMA  || key == SDLK_x) joypad_set_button(m, JP_B, 1, 1);
			if (key == SDLK_BACKSPACE) joypad_set_button(m, JP_SELECT, 1, 1);
			if (key == SDLK_RETURN) joypad_set_button(m, JP_START, 1, 1);

			// emulation
			if (key == SDLK_o) 
				s->new_game = 1;
			if (key == SDLK_r) 
				s->restart_triggered = 1;

			if (key == SDLK_n){
				
				if (mods & KMOD_LSHIFT) s->advance_cycle = 1; 
				else s->advance_frame = 1;
				s->paused = 0;
			}
			
			if (key == SDLK_p) 
				s->paused = !s->paused;
			if (key == SDLK_v) 
				s->show_vram_viewer = !s->show_vram_viewer;
			// logging
			if (key == SDLK_l){
				if(!s->print_frame) print_table_header(s); 
				s->print_frame = !s->print_frame;}
			if (key == SDLK_m) 
				s->print_memory = 1;
			// speed
			if (key == SDLK_f) 
				s->fast_forward = 1;

		} 
		if (e.type == SDL_KEYDOWN) { // allow key to be held down
			if (key == SDLK_RSHIFT) 
				s->print_cycle = 1;
		}
		if (e.type == SDL_KEYUP) { // for held keys, not toggled keys
			//controls
			if (key == SDLK_w || key == SDLK_UP) joypad_set_button(m, JP_UP, 0, 0);
			if (key == SDLK_a || key == SDLK_LEFT) joypad_set_button(m, JP_LEFT, 0, 0);
			if (key == SDLK_s || key == SDLK_DOWN) joypad_set_button(m, JP_DOWN, 0, 0);
			if (key == SDLK_d || key == SDLK_RIGHT) joypad_set_button(m, JP_RIGHT, 0, 0);

			if (key == SDLK_PERIOD || key == SDLK_z) joypad_set_button(m, JP_A, 0, 1);
			if (key == SDLK_COMMA  || key == SDLK_x) joypad_set_button(m, JP_B, 0, 1);
			if (key == SDLK_BACKSPACE) joypad_set_button(m, JP_SELECT, 0, 1);
			if (key == SDLK_RETURN) joypad_set_button(m, JP_START, 0, 1);

			//logging
			if (key == SDLK_RSHIFT) 
				s->print_cycle = 0; // only print on hold, do not toggle

			// speed
			if (key == SDLK_f) 
				s->fast_forward = 0; // only ff on hold, do not toggle
		}
		
	}
}

void window_destroy(void) {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void pause_framebuffer(uint32_t* framebuffer) {
	// adds pause icon to screen
	
	int row = 160; // pixels in row
	
	// left part of pause icon
	for (int i = 2;i<8;i++){
		framebuffer[row*2 + i] = 0xFFFF0000;
		framebuffer[row*3 + i] = 0xFFFF0000;
	}
	
	for (int i = 4;i<12;i++){
		framebuffer[row*i + 2] = 0xFFFF0000;
		framebuffer[row*i + 3] = 0xFFFF0000;
		framebuffer[row*i + 4] = 0xFFFFFFFF;
		framebuffer[row*i + 5] = 0xFFFFFFFF;
		framebuffer[row*i + 6] = 0xFFFF0000;
		framebuffer[row*i + 7] = 0xFFFF0000;
	}
	
	for (int i = 2;i<8;i++){
		framebuffer[row*12 + i] = 0xFFFF0000;
		framebuffer[row*13 + i] = 0xFFFF0000;
	}
	
	// right part of pause icon
	for (int i = 10;i<16;i++){
		framebuffer[row*2 + i] = 0xFFFF0000;
		framebuffer[row*3 + i] = 0xFFFF0000;
	}
	
	for (int i = 4;i<12;i++){
		framebuffer[row*i + 10] = 0xFFFF0000;
		framebuffer[row*i + 11] = 0xFFFF0000;
		framebuffer[row*i + 12] = 0xFFFFFFFF;
		framebuffer[row*i + 13] = 0xFFFFFFFF;
		framebuffer[row*i + 14] = 0xFFFF0000;
		framebuffer[row*i + 15] = 0xFFFF0000;
	}
	
	for (int i = 10;i<16;i++){
		framebuffer[row*12 + i] = 0xFFFF0000;
		framebuffer[row*13 + i] = 0xFFFF0000;
	}
}