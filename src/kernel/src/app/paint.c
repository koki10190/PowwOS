#include <app/paint.h>
#include <app/import_apps.h>
#include <math/math.h>
#include <uart.h>
#include <string.h>
#include <memory.h>
#include <display/window.h>
#include <display/display.h>
#include <graphics/graphics.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <time/rtc.h>

static window_t *paint_window;
#define PAINT_RECT_WIDTH 256
#define PAINT_RECT_HEIGHT 256
static uint32_t rect_pixels[PAINT_RECT_WIDTH * PAINT_RECT_HEIGHT];
static uint32_t current_color = BLACK;
static int pixel_amout = 4;

void paint_update() {
    for (int yy = 0; yy < PAINT_RECT_HEIGHT; yy++) {
        int y_pos = paint_window->position.y + tb_height + yy;
        for (int xx = 0; xx < PAINT_RECT_WIDTH; xx++) {
            int x_pos = paint_window->position.x + xx;
            if (mouse.position.x == x_pos && mouse.position.y == y_pos && mouse.left_clicked) {
                rect_pixels[xx + yy * PAINT_RECT_WIDTH] = current_color;
                /*
                0, 0
                1, 0
                */
                rect_pixels[(xx + 1) + yy * PAINT_RECT_WIDTH] = current_color;
                /*
                0, 0
                1, 1
                */
                rect_pixels[(xx + 1) + (yy + 1) * PAINT_RECT_WIDTH] = current_color;
                /*
                0, 1
                1, 1
                */
                rect_pixels[xx + (yy + 1) * PAINT_RECT_WIDTH] = current_color;
                /*
                1, 1
                1, 1
                */
                rect_pixels[(xx - 1) + (yy + 1) * PAINT_RECT_WIDTH] = current_color;
                /*
                1, 1, 1
                0, 1, 1
                */
                rect_pixels[(xx - 1) + yy * PAINT_RECT_WIDTH] = current_color;
                /*
                1, 1, 1
                1, 1, 1
                */
                rect_pixels[(xx - 1) + (yy - 1) * PAINT_RECT_WIDTH] = current_color;
                /*
                1, 1, 1
                1, 1, 1
                1, 0, 0
                */
                rect_pixels[(xx) + (yy - 1) * PAINT_RECT_WIDTH] = current_color;
                /*
                1, 1, 1
                1, 1, 1
                1, 1, 0
                */
                rect_pixels[(xx + 1) + (yy - 1) * PAINT_RECT_WIDTH] = current_color;
                /*
                1, 1, 1
                1, 1, 1
                1, 1, 1
                */

                // if (xx + 1 < PAINT_RECT_WIDTH - 1) {

                //     if (yy + 1 < PAINT_RECT_HEIGHT - 1) {
                //     }
                // }

                // if (yy + 1 < PAINT_RECT_HEIGHT - 1) {
                // }

            } else if (mouse.position.x == x_pos && mouse.position.y == y_pos && mouse.right_clicked) {
                rect_pixels[xx + yy * PAINT_RECT_WIDTH] = WHITE;
                /*
                0, 0
                1, 0
                */
                rect_pixels[(xx + 1) + yy * PAINT_RECT_WIDTH] = WHITE;
                /*
                0, 0
                1, 1
                */
                rect_pixels[(xx + 1) + (yy + 1) * PAINT_RECT_WIDTH] = WHITE;
                /*
                0, 1
                1, 1
                */
                rect_pixels[xx + (yy + 1) * PAINT_RECT_WIDTH] = WHITE;
                /*
                1, 1
                1, 1
                */
                rect_pixels[(xx - 1) + (yy + 1) * PAINT_RECT_WIDTH] = WHITE;
                /*
                1, 1, 1
                0, 1, 1
                */
                rect_pixels[(xx - 1) + yy * PAINT_RECT_WIDTH] = WHITE;
                /*
                1, 1, 1
                1, 1, 1
                */
                rect_pixels[(xx - 1) + (yy - 1) * PAINT_RECT_WIDTH] = WHITE;
                /*
                1, 1, 1
                1, 1, 1
                1, 0, 0
                */
                rect_pixels[(xx) + (yy - 1) * PAINT_RECT_WIDTH] = WHITE;
                /*
                1, 1, 1
                1, 1, 1
                1, 1, 0
                */
                rect_pixels[(xx + 1) + (yy - 1) * PAINT_RECT_WIDTH] = WHITE;
                /*
                1, 1, 1
                1, 1, 1
                1, 1, 1
                */
            }

            plot_pixel_buffer(x_pos, y_pos, rect_pixels[xx + yy * PAINT_RECT_WIDTH], back_buffer);
        }
    }
    // window_render_rect(paint_window, 0, 0, PAINT_RECT_WIDTH, PAINT_RECT_WIDTH, WHITE);
}

void paint_keyboard_int(uint8_t scancode) {

    switch (scancode) {
    case KEY_1:
        current_color = BLUE;
        break;
    case KEY_2:
        current_color = LIGHTBLUE;
        break;
    case KEY_3:
        current_color = GREEN;
        break;
    case KEY_4:
        current_color = LIGHTGREEN;
        break;
    case KEY_5:
        current_color = RED;
        break;
    case KEY_6:
        current_color = LIGHTRED;
        break;
    case KEY_7:
        current_color = MAGENTA;
        break;
    case KEY_8:
        current_color = PINK;
        break;
    case KEY_9:
        current_color = LIGHTGRAY;
        break;
    case KEY_0:
        current_color = DARKGRAY;
        break;
    case KEY_MINUS:
        current_color = WHITE;
        break;
    case KEY_EQUAL:
        current_color = BLACK;
        break;
    case KEY_DELETE:
        for (int yy = 0; yy < PAINT_RECT_HEIGHT; yy++) {
            for (int xx = 0; xx < PAINT_RECT_WIDTH; xx++) {
                rect_pixels[xx + yy * PAINT_RECT_WIDTH] = WHITE;
            }
        }
        break;
    }
}

void paint_init() {
    for (int yy = 0; yy < PAINT_RECT_HEIGHT; yy++) {
        for (int xx = 0; xx < PAINT_RECT_WIDTH; xx++) {
            rect_pixels[xx + yy * PAINT_RECT_WIDTH] = WHITE;
        }
    }
    if (!paint_window) {
        paint_window = &windows[window_init((vector2_t){150, 150}, (vector2_t){256, 256 + tb_height}, "Paint", paint_update)];
        paint_window->active = true;
        paint_window->background_color = 0x0A0A0A;
        paint_window->keyboard_interrupt_function = paint_keyboard_int;
        paint_window->title_bar_color = 0x73ff69;
        focused_window = paint_window;
    } else {
        paint_window->active = true;
    }
}
