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

    return renderer && texture;
}

void window_update(uint32_t* framebuffer) {
    SDL_UpdateTexture(texture, NULL, framebuffer, GB_WIDTH * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void check_events(Status* s){	

	/*
	=== KEYBINDS ===
	
	CONTROLS
	w: up dpad
	a: left dpad
	s: down dpad
	d: right dpad
	,: b
	.: a
	enter: start
	backspace: select
	
	EMULATION
	p: toggle pause
	v: toggle vram viewer
	
	LOGGING
	l: enable logging every frame
	m: print memory to console
	right shift: logging every instruction (VERY SLOW)
	
	SPEED
	-: slow down emulation
	+: speed up emulation
	
	*/

	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) {
			window_destroy();
			s->paused = 0;
			s->running = 0;
		}
		if (e.type == SDL_KEYDOWN) {
			// controls
			if (e.key.keysym.sym == SDLK_w); 
			if (e.key.keysym.sym == SDLK_a); 
			if (e.key.keysym.sym == SDLK_s); 
			if (e.key.keysym.sym == SDLK_d); 
			if (e.key.keysym.sym == SDLK_COMMA); 
			if (e.key.keysym.sym == SDLK_PERIOD); 
			if (e.key.keysym.sym == SDLK_RETURN); 
			if (e.key.keysym.sym == SDLK_BACKSPACE); 
			// emulation
			if (e.key.keysym.sym == SDLK_p && !e.key.repeat) 
				s->paused = !s->paused;
			if (e.key.keysym.sym == SDLK_v && !e.key.repeat) 
				s->show_vram_viewer = !s->show_vram_viewer;
			// logging
			if (e.key.keysym.sym == SDLK_l && !e.key.repeat){
				if(!s->print_frame) print_table_header(s); 
				s->print_frame = !s->print_frame;}
			if (e.key.keysym.sym == SDLK_m && !e.key.repeat) 
				s->print_memory = 1;
			if (e.key.keysym.sym == SDLK_RSHIFT) 
				s->print_cycle = 1;
		}
		if (e.type == SDL_KEYUP) { // for held keys, not toggled keys
			if (e.key.keysym.sym == SDLK_RSHIFT) 
				s->print_cycle = 0; // only print on hold, do not toggle
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