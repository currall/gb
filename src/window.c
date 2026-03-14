
#include <SDL2/SDL.h>

#include "debug.h"
#include "memory.h"
#include "palette.h"
#include "window.h"
#include "ui.h"

static SDL_Window*   window   = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture*  texture  = NULL;
static SDL_Texture*  ui_texture=NULL;
uint32_t window_id = 0;

void controller_init(){
	SDL_GameControllerEventState(SDL_ENABLE);
	SDL_GameController* controller = NULL;

	for (int i = 0; i < SDL_NumJoysticks(); i++) {
		if (SDL_IsGameController(i)) {
			controller = SDL_GameControllerOpen(i);
			if (controller) {
				printf("[INPUT] Opened controller: %s\n", SDL_GameControllerName(controller));
				break;
			}
		}
	}
}

void ui_init() {
	ui_texture  = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        GB_WIDTH,
        GB_HEIGHT
    );
	SDL_SetTextureBlendMode(ui_texture, SDL_BLENDMODE_BLEND);

	clear_ui_framebuffer();
}

int window_init(char* file) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0)
        return 0;
	
	char title[256];
	snprintf(title, sizeof(title), "gb - %s", file);

    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        GB_WIDTH * 4,
        GB_HEIGHT * 4,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
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
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
	
	window_id = SDL_GetWindowID(window);
	SDL_RaiseWindow(window);
	SDL_SetWindowInputFocus(window);

	controller_init(); // use controller

    return renderer && texture;
}

void toggle_fullscreen(Status* s) {
    s->fullscreen = !s->fullscreen;

    if (s->fullscreen)
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    else
        SDL_SetWindowFullscreen(window, 0);
}
void set_window_size(int scale) {SDL_SetWindowSize(window, GB_WIDTH * scale, GB_HEIGHT * scale);}

void get_window_size(SDL_Rect* output)
{
    int window_w, window_h;
    SDL_GetRendererOutputSize(renderer, &window_w, &window_h); // gets window size

    float target_aspect = (float)GB_WIDTH / (float)GB_HEIGHT;
    float window_aspect = (float)window_w / (float)window_h;

    if (window_aspect > target_aspect) { // window too wide
        output->h = window_h;
        output->w = (int)(window_h * target_aspect);
        output->x = (window_w - output->w) / 2;
        output->y = 0;
    } else { // window too tall
        output->w = window_w;
        output->h = (int)(window_w / target_aspect);
        output->x = 0;
        output->y = (window_h - output->h) / 2;
    }
}

void window_update(uint32_t* framebuffer){
    SDL_UpdateTexture(texture, NULL, framebuffer, GB_WIDTH * sizeof(uint32_t));

	// scale output to window
    SDL_Rect output;
    get_window_size(&output);

    SDL_RenderClear(renderer); 
    SDL_RenderCopy(renderer, texture, NULL, &output);
    SDL_RenderPresent(renderer);
}
void ui_window_update(uint32_t* ui_framebuffer){
    SDL_UpdateTexture(ui_texture, NULL, ui_framebuffer, GB_WIDTH * sizeof(uint32_t));

	// scale output to window
    SDL_Rect output;
    get_window_size(&output);

    SDL_RenderClear(renderer); 
    SDL_RenderCopy(renderer, texture, NULL, &output);   // redraw game
    SDL_RenderCopy(renderer, ui_texture, NULL, &output); // overlay ui
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
		//printf("[INPUT] Joypad input detected!\n");
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

			// function keys
			if (key == SDLK_F1) {
				joypad_set_button(m, JP_A, 1, 1);
				joypad_set_button(m, JP_B, 1, 1);
				joypad_set_button(m, JP_SELECT, 1, 1);
				joypad_set_button(m, JP_START, 1, 1);
			}
			if (key == SDLK_F11) toggle_fullscreen(s);

			// ctrl bindings
			if (mods & KMOD_CTRL) { // emulation control

				// file loading
				if (key == SDLK_o) s->new_game = 1;
				if (key == SDLK_r) s->restart_triggered = 1;

				// pause and speed
				if (key == SDLK_f) s->fast_forward = !s->fast_forward;
				if (key == SDLK_n){
					if (mods & KMOD_LSHIFT) s->advance_cycle = 1; 
					else s->advance_frame = 1;
					s->paused = 0;
				}
				if (key == SDLK_p) s->paused = !s->paused;
				// logging
				if (key == SDLK_l){
					if(!s->print_frame) print_table_header(s); // print header if first frame printed
				
					if (mods & KMOD_LSHIFT) s->print_cycle = !s->print_cycle; 
					else s->print_frame = !s->print_frame; 
				}
				if (key == SDLK_m) s->print_memory = 1;

				// video
				if (key == SDLK_v) s->show_vram_viewer = !s->show_vram_viewer;
				switch (key) { // set window size gameboy resolution * key
					case SDLK_1: set_window_size(1); break; 
					case SDLK_2: set_window_size(2); break; 
					case SDLK_3: set_window_size(3); break;
					case SDLK_4: set_window_size(4); break;
					case SDLK_5: set_window_size(5); break;
					case SDLK_6: set_window_size(6); break;
					case SDLK_7: set_window_size(7); break;
					case SDLK_8: set_window_size(8); break;
					case SDLK_9: set_window_size(9); break;
				} 
			}

			// alt bindings
			else if (mods & KMOD_ALT) {
				switch (key) {
					case SDLK_1: apply_palette(m->ppu, 0, 1); break; // black and white
					case SDLK_2: apply_palette(m->ppu, s->game_id, 0); break; // game specific palette
					case SDLK_3: apply_palette(m->ppu, 1, 1); break;
					case SDLK_4: apply_palette(m->ppu, 2, 1); break;
					case SDLK_5: apply_palette(m->ppu, ID_TETRIS, 0); break;
					case SDLK_6: apply_palette(m->ppu, ID_ALLEYWAY, 0); break;
					case SDLK_7: apply_palette(m->ppu, ID_MARIO_LAND_2, 0); break;
					case SDLK_8: apply_palette(m->ppu, ID_WARIO_LAND, 0); break;
					case SDLK_9: apply_palette(m->ppu, ID_DR_MARIO, 0); break;
					case SDLK_0: apply_palette(m->ppu, ID_POKEMONRED, 0); break;
				} 
				
				if (key == SDLK_RETURN) toggle_fullscreen(s);
			}

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

			if (key == SDLK_F1) {
				joypad_set_button(m, JP_A, 0, 1);
				joypad_set_button(m, JP_B, 0, 1);
				joypad_set_button(m, JP_SELECT, 0, 1);
				joypad_set_button(m, JP_START, 0, 1);
			}
		}
		if (e.type == SDL_CONTROLLERBUTTONDOWN) {
			switch (e.cbutton.button) {
				case SDL_CONTROLLER_BUTTON_DPAD_UP: joypad_set_button(m, JP_UP, 1, 0); break;
				case SDL_CONTROLLER_BUTTON_DPAD_DOWN: joypad_set_button(m, JP_DOWN, 1, 0); break;
				case SDL_CONTROLLER_BUTTON_DPAD_LEFT: joypad_set_button(m, JP_LEFT, 1, 0); break;
				case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: joypad_set_button(m, JP_RIGHT, 1, 0); break;
				case SDL_CONTROLLER_BUTTON_A: joypad_set_button(m, JP_A, 1, 1); break;
				case SDL_CONTROLLER_BUTTON_B: joypad_set_button(m, JP_B, 1, 1); break;
				case SDL_CONTROLLER_BUTTON_START: joypad_set_button(m, JP_START, 1, 1); break;
				case SDL_CONTROLLER_BUTTON_BACK: joypad_set_button(m, JP_SELECT, 1, 1); break;
			}
		}
		if (e.type == SDL_CONTROLLERBUTTONUP) {
			switch (e.cbutton.button) {
				case SDL_CONTROLLER_BUTTON_DPAD_UP: joypad_set_button(m, JP_UP, 0, 0); break;
				case SDL_CONTROLLER_BUTTON_DPAD_DOWN: joypad_set_button(m, JP_DOWN, 0, 0); break;
				case SDL_CONTROLLER_BUTTON_DPAD_LEFT: joypad_set_button(m, JP_LEFT, 0, 0); break;
				case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: joypad_set_button(m, JP_RIGHT, 0, 0); break;
				case SDL_CONTROLLER_BUTTON_A: joypad_set_button(m, JP_A, 0, 1); break;
				case SDL_CONTROLLER_BUTTON_B: joypad_set_button(m, JP_B, 0, 1); break;
				case SDL_CONTROLLER_BUTTON_START: joypad_set_button(m, JP_START, 0, 1); break;
				case SDL_CONTROLLER_BUTTON_BACK: joypad_set_button(m, JP_SELECT, 0, 1); break;
			}
		}
	}
	update_ui(s);
}

void window_destroy(void) {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

