#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

extern int text_start_x, text_start_y;
extern int cursor_x, cursor_y;

void render_background(uint32_t color);
void render_topbar();
void print(char *buf, uint32_t color);
void println(char *buf, uint32_t color);

#endif