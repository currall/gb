//window.h
#ifndef WINDOW_H
#define WINDOW_H

#include <stdint.h>

typedef struct Status Status;

#define GB_WIDTH  160
#define GB_HEIGHT 144

// DPAD
#define JP_RIGHT  (1 << 0)
#define JP_LEFT   (1 << 1)
#define JP_UP     (1 << 2)
#define JP_DOWN   (1 << 3)

// BUTTONS
#define JP_A      (1 << 0)
#define JP_B      (1 << 1)
#define JP_SELECT (1 << 2)
#define JP_START  (1 << 3)

int  window_init(char* file, Status* s);
void window_update(uint32_t* framebuffer);
void ui_window_update(uint32_t* ui_framebuffer);

void check_events(Status* s, Memory* m);
void window_destroy(void);

#endif