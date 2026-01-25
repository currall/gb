
#include "memory.h"
#include "vram_window.h"

#include <SDL2/SDL.h>
#include <stdint.h>

#define TILES_PER_ROW 16
#define TILE_SIZE     8
#define TILE_COUNT    384

#define VRAM_WIDTH  (TILES_PER_ROW * TILE_SIZE)   // 128
#define VRAM_HEIGHT ((TILE_COUNT / TILES_PER_ROW) * TILE_SIZE) // 192

uint32_t vram_palette[4] = {0xFFFFFFFF, 0xFFC0C0C0, 0xFF606060, 0xFF000000};

static SDL_Window*   vram_window   = NULL;
static SDL_Renderer* vram_renderer = NULL;
static SDL_Texture*  vram_texture  = NULL;
uint32_t vram_window_id;

void draw_vram(Memory* m, uint32_t* framebuffer) {

    uint8_t* vram = m->memory + 0x8000;

    for (int tile = 0; tile < 384; tile++) {
        int tile_x = (tile % TILES_PER_ROW) * TILE_SIZE;
        int tile_y = (tile / TILES_PER_ROW) * TILE_SIZE;

        uint8_t* tile_data = vram + tile * 16;

        for (int y = 0; y < 8; y++) {
            uint8_t lo = tile_data[y * 2 + 0];
            uint8_t hi = tile_data[y * 2 + 1];

            for (int x = 0; x < 8; x++) {
                int bit = 7 - x;
                uint8_t color =
                    ((hi >> bit) & 1) << 1 |
                    ((lo >> bit) & 1);

                int px = tile_x + x;
                int py = tile_y + y;
				
				// read palette like gameboy
				uint8_t bgp = read8(m, 0xFF47);
				int shade = (bgp >> (color * 2)) & 0x03;

                framebuffer[py * VRAM_WIDTH + px] = vram_palette[shade];
            }
        }
    }
}

int vram_window_init(void) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        return 0;

    vram_window = SDL_CreateWindow(
        "vram viewer",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        VRAM_WIDTH * 2,
        VRAM_HEIGHT * 2,
        SDL_WINDOW_SHOWN
    );

    if (!vram_window) return 0;

    vram_renderer = SDL_CreateRenderer(vram_window, -1, SDL_RENDERER_ACCELERATED);
    vram_texture  = SDL_CreateTexture(
        vram_renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        VRAM_WIDTH,
        VRAM_HEIGHT
    );
	vram_window_id = SDL_GetWindowID(vram_window);

    return vram_renderer && vram_texture;
}

void vram_window_update(Memory* m) {
	
	uint32_t flags = SDL_GetWindowFlags(vram_window);

	if (flags & SDL_WINDOW_HIDDEN) {
		vram_window_show();
	}
	
	uint32_t framebuffer[VRAM_WIDTH*VRAM_HEIGHT] = {0};
	draw_vram(m, framebuffer);
	
    SDL_UpdateTexture(vram_texture, NULL, framebuffer, VRAM_WIDTH * sizeof(uint32_t));
    SDL_RenderClear(vram_renderer);
    SDL_RenderCopy(vram_renderer, vram_texture, NULL, NULL);
    SDL_RenderPresent(vram_renderer);
}

void vram_window_show(void) {
    SDL_ShowWindow(vram_window);
}
void vram_window_hide(void) {
    SDL_HideWindow(vram_window);
}


void vram_window_destroy(void) {
    SDL_DestroyTexture(vram_texture);
    SDL_DestroyRenderer(vram_renderer);
    SDL_DestroyWindow(vram_window);
}
