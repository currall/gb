#include <SDL2/SDL.h>

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) return 1;
	
	SDL_Window* window;
	SDL_Renderer* renderer;	
	
    window = SDL_CreateWindow("gb", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 160, 144, 0);
	renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);
	
    if (!w) { SDL_Quit(); return 1; }

    SDL_Event e;
    int running = 1;
    while (running) {
		// events
        while (SDL_PollEvent(&e))
            if (e.type == SDL_QUIT)
				running = 0;
			
		// draw
		SDL_Rect rect = 
			
		// clock
        SDL_Delay(10);
    }

    SDL_DestroyWindow(w);
    SDL_Quit();
    return 0;
}
