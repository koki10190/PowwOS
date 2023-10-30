#ifndef WINDOW_H
#define WINDOW_H

#include <math/math.h>
#include <graphics/graphics.h>

typedef struct __window_t {
    vector2_t position;
    vector2_t size;
    char *title;
    uint32_t background_color;
    uint32_t title_bar_color;
    void *update_function;
    void *keyboard_interrupt_function;
    bool active;
} window_t;

typedef struct __button_t {
    vector2_t position;
    vector2_t size;
    char *label;
    uint32_t background_color;
    uint32_t hover_bg_color;
    uint32_t text_color;

    void *on_hover;
    void *on_exit_hover;
    void *on_click;
} button_t;

extern window_t windows[128];
extern int window_count;
extern vector2_t *currently_dragging;
extern window_t *focused_window;
extern int tb_height;

int window_init(vector2_t position, vector2_t size, char *title, void *update_function);
void window_close(window_t *window);
void window_open(window_t *window);
void button_init(button_t *button, vector2_t position, vector2_t size, char *label, uint32_t background_color, uint32_t hover_bg_color, uint32_t text_color);
void button_events(window_t *window, button_t *button);
void window_manager_handler();
void render_window(window_t *window);

void window_render_char(window_t *window, char c, int x, int y, uint32_t color);
void window_render_text(window_t *window, char *s, int x, int y, uint32_t color);
void window_render_rect(window_t *window, int x, int y, int width, int height, uint32_t color);
void window_render_button(window_t *window, button_t *button);
void window_render_text_margin(window_t *window, char *s, int x, int y, uint32_t color, int margin_x, int margin_y);
void window_render_char_margin(window_t *window, char c, int x, int y, uint32_t color, int margin_x, int margin_y);

#endif