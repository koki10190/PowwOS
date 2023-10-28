#include <app/bg_color.h>
#include <app/import_apps.h>
#include <math/math.h>
#include <uart.h>
#include <string.h>
#include <memory.h>
#include <display/window.h>
#include <display/display.h>
#include <graphics/graphics.h>
#include <drivers/keyboard.h>
#include <time/rtc.h>
#include <sprites.h>

static window_t *bg_color_window;

void lred_click() {
    display_color = LIGHTRED;
    render_background(display_color);
}
void lgreen_click() {
    display_color = LIGHTGREEN;
    render_background(display_color);
}
void blue_click() {
    display_color = LIGHTBLUE;
    render_background(display_color);
}
void default_click() {
    display_color = 0x22bbdd;
    render_background(display_color);
}
void black_click() {
    display_color = BLACK;
    render_background(display_color);
}
void white_click() {
    display_color = WHITE;
    render_background(display_color);
}

void bg_color_update() {
    button_t lred, lgreen, blue, _default, black, white;

    int btn_size = 100;

    button_init(&lred, (vector2_t){15, 15}, (vector2_t){btn_size, btn_size}, "RED", LIGHTRED, RED, WHITE);
    button_init(&lgreen, (vector2_t){(15 + (btn_size * 1) + (10 * 1)), 15}, (vector2_t){btn_size, btn_size}, "GREEN", LIGHTGREEN, GREEN, WHITE);
    button_init(&blue, (vector2_t){(15 + (btn_size * 2) + (10 * 2)), 15}, (vector2_t){btn_size, btn_size}, "BLUE", LIGHTBLUE, BLUE, WHITE);

    // row two
    button_init(&_default, (vector2_t){15, 15 + btn_size + 10}, (vector2_t){btn_size, btn_size}, "DEFAULT", 0x22bbdd, 0x177d94, WHITE);
    button_init(&black, (vector2_t){(15 + (btn_size * 1) + (10 * 1)), 15 + btn_size + 10}, (vector2_t){btn_size, btn_size}, "BLACK", 0x121212, 0x181818, WHITE);
    button_init(&white, (vector2_t){(15 + (btn_size * 2) + (10 * 2)), 15 + btn_size + 10}, (vector2_t){btn_size, btn_size}, "WHITE", WHITE, 0xDDDDDD, BLACK);
    lred.on_click = lred_click;
    lgreen.on_click = lgreen_click;
    blue.on_click = blue_click;
    _default.on_click = default_click;
    black.on_click = black_click;
    white.on_click = white_click;

    window_render_button(bg_color_window, &lred);
    window_render_button(bg_color_window, &lgreen);
    window_render_button(bg_color_window, &blue);

    // row two
    window_render_button(bg_color_window, &_default);
    window_render_button(bg_color_window, &black);
    window_render_button(bg_color_window, &white);
}

void bg_color_init() {
    if (!bg_color_window) {
        bg_color_window = &windows[window_init((vector2_t){15, 40}, (vector2_t){350, 260}, "Background Color", bg_color_update)];
        bg_color_window->active = true;
        bg_color_window->background_color = 0x0A0A0A;
        focused_window = bg_color_window;
    } else {
        bg_color_window->active = true;
    }
}