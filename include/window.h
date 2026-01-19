#ifndef WINDOW_H
#define WINDOW_H

#include <stdint.h>

#define GB_WIDTH  160
#define GB_HEIGHT 144

int  window_init(void);
void window_update(const uint32_t* framebuffer, int* running);
void window_destroy(void);

#endif