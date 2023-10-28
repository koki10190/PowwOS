#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <math/math.h>

#define BLUE 0x0000ff
#define LIGHTBLUE 0x504aff
#define GREEN 0x0c9400
#define CYAN 0x00FFFF
#define LIGHTCYAN 0x69ffff
#define MAGENTA 0xFF00FF
#define PINK 0xf454ff
#define LIGHTMAGENTA 0xf454ff
#define BROWN 0x0000ff
#define LIGHTGRAY 0x9c9c9c
#define DARKGRAY 0x333333
#define LIGHTGREEN 0x1eff00
#define LIGHTRED 0xff3d3d
#define RED 0xff0000
#define YELLOW 0xffee00
#define WHITE 0xFFFFFF
#define BLACK 0x000000

extern const uint8_t bitmap_1[128][8];
extern uint32_t back_buffer[1280 * 1080];
extern uint32_t mouse_cursor_buffer[16 * 16];

void graphics_init();
void swap_buffers();
void plot_pixel_buffer(int x, int y, uint32_t color, uint32_t *pixel_buffer);
void plot_pixel(int x, int y, uint32_t color);
uint32_t get_pixel(int x, int y);

void render_char(char c, size_t x, size_t y, uint32_t color);
void render_str(const char *s, size_t x, size_t y, uint32_t color);
void render_text(const char *s, size_t x, size_t y, uint32_t color);
void render_delchar(char c, size_t x, size_t y, uint32_t color);
void render_mouse_cursor(uint8_t *mouse_cursor, vector2_t pos, uint32_t color);
void clear_mouse_cursor(uint8_t *mouse_cursor, vector2_t pos);
void render_text_bb(const char *s, size_t x, size_t y, uint32_t color);
bool collision(vector2_t pos1, vector2_t size1, vector2_t pos2, vector2_t size2);

#endif