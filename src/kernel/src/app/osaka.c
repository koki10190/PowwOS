#include <app/osaka.h>
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

static window_t *osaka_window;

void osaka_update() {
    render_osaka_icon_sprite(osaka_window->position.x, osaka_window->position.y + 20, WHITE);
}

void osaka_init() {
    if (!osaka_window) {
        osaka_window = &windows[window_init((vector2_t){150, 150}, (vector2_t){126, 159 + 20}, "OMYGAAH", osaka_update)];
        osaka_window->active = true;
        osaka_window->background_color = 0x0A0A0A;
        focused_window = osaka_window;
    } else {
        osaka_window->active = true;
    }
}