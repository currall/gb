//window.h
#ifndef WINDOW_H
#define WINDOW_H

#include <stdint.h>

#define GB_WIDTH  160
#define GB_HEIGHT 144

int  window_init(char* file);
void window_update(uint32_t* framebuffer);
void check_events(int* running, int* paused, int* vram_viewer);
void window_destroy(void);
void pause_framebuffer(uint32_t* framebuffer);

#endif