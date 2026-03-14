#include <SDL2/SDL.h>

#include "debug.h"
#include "memory.h"
#include "window.h"

uint32_t ui_framebuffer[160*144];

uint32_t* ui_get_framebuffer() {
    return ui_framebuffer;
}

void clear_ui_framebuffer(){
	for (int i = 0; i < (160*144); i++){
		ui_framebuffer[i] = 0;
	}
}

void pause_framebuffer() {
	// adds pause icon to screen
	
	int row = 160; // pixels in row
	
	// left part of pause icon
	for (int i = 2;i<8;i++){
		ui_framebuffer[row*2 + i] = 0xFFFF0000;
		ui_framebuffer[row*3 + i] = 0xFFFF0000;
	}
	
	for (int i = 4;i<12;i++){
		ui_framebuffer[row*i + 2] = 0xFFFF0000;
		ui_framebuffer[row*i + 3] = 0xFFFF0000;
		ui_framebuffer[row*i + 4] = 0xFFFFFFFF;
		ui_framebuffer[row*i + 5] = 0xFFFFFFFF;
		ui_framebuffer[row*i + 6] = 0xFFFF0000;
		ui_framebuffer[row*i + 7] = 0xFFFF0000;
	}
	
	for (int i = 2;i<8;i++){
		ui_framebuffer[row*12 + i] = 0xFFFF0000;
		ui_framebuffer[row*13 + i] = 0xFFFF0000;
	}
	
	// right part of pause icon
	for (int i = 10;i<16;i++){
		ui_framebuffer[row*2 + i] = 0xFFFF0000;
		ui_framebuffer[row*3 + i] = 0xFFFF0000;
	}
	
	for (int i = 4;i<12;i++){
		ui_framebuffer[row*i + 10] = 0xFFFF0000;
		ui_framebuffer[row*i + 11] = 0xFFFF0000;
		ui_framebuffer[row*i + 12] = 0xFFFFFFFF;
		ui_framebuffer[row*i + 13] = 0xFFFFFFFF;
		ui_framebuffer[row*i + 14] = 0xFFFF0000;
		ui_framebuffer[row*i + 15] = 0xFFFF0000;
	}
	
	for (int i = 10;i<16;i++){
		ui_framebuffer[row*12 + i] = 0xFFFF0000;
		ui_framebuffer[row*13 + i] = 0xFFFF0000;
	}
}

void update_ui(Status* s){

	clear_ui_framebuffer();
	if (s->paused) pause_framebuffer();
    ui_window_update(ui_framebuffer);

}