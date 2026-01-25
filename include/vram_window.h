//vram_window.h
#ifndef VRAM_WINDOW_H
#define VRAM_WINDOW_H

#include <stdint.h>

int  vram_window_init(void);
void vram_window_update(Memory* m);
void vram_window_show(void);
void vram_window_hide(void);
void vram_window_destroy(void);

#endif