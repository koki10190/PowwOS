#ifndef SPRITES_H
#define SPRITES_H

#include <stdint.h>

void render_x_sprite(int x, int y, uint32_t color);
void render_calc_icon_sprite(int x, int y, uint32_t color);
void render_terminal_icon_sprite(int x, int y, uint32_t color);
void render_osaka_icon_sprite(int x, int y, uint32_t color);
void render_desktop_bg_sprite(int x, int y, uint32_t color);
void render_undefined_sprite(int x, int y, uint32_t *bitmap, int width, int height);

#endif