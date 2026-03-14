//window.h
#ifndef UI_H
#define UI_H

#include <stdint.h>

typedef struct Status Status;

uint32_t* ui_get_framebuffer();
void clear_ui_framebuffer();

void update_ui(Status* s);

#endif