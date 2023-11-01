#include <display/window.h>
#include <string.h>
#include <drivers/mouse.h>
#include <uart.h>
#include <boot.h>
#include <sprites.h>

window_t windows[128];
int window_count = 0;
vector2_t *currently_dragging;
window_t *focused_window;

uint32_t window_buffer[500 * 500];
int tb_height = 20;

void window_render_char(window_t *window, char c, int x, int y, uint32_t color) {
    uint8_t *glyph = NULL;

    if (kernel_info->font) {
        glyph = kernel_info->font->glyph_buffer + (c * kernel_info->font->header->charsize);
    } else {
        glyph = bitmap_1[(size_t)c];
    }

    for (size_t yy = 0; yy < kernel_info->font->height; yy++) {
        if (window->position.y + tb_height + yy + y > window->position.y + window->size.y - 1)
            continue;
        for (size_t xx = 0; xx < kernel_info->font->width; xx++) {
            if (kernel_info->font) {
                if (window->position.x + xx + x > window->position.x + window->size.x - 1)
                    continue;
                if (((*glyph & (0b10000000 >> ((xx + x) - x)))) > 0) {
                    plot_pixel_buffer(window->position.x + x + xx, window->position.y + tb_height + y + yy, color, back_buffer);
                }
            } else {
                if (window->position.x + xx + x > window->position.x + window->size.x - 1)
                    continue;
                if (glyph[yy] & (1 << xx)) {
                    plot_pixel_buffer(window->position.x + x + xx, window->position.y + tb_height + y + yy, color, back_buffer);
                }
            }
        }
        if (kernel_info->font) {
            glyph++;
        }
    }
}
void window_render_char_margin(window_t *window, char c, int x, int y, uint32_t color, int margin_x, int margin_y) {
    uint8_t *glyph = NULL;

    if (kernel_info->font) {
        glyph = kernel_info->font->glyph_buffer + (c * kernel_info->font->header->charsize);
    } else {
        glyph = bitmap_1[(size_t)c];
    }
    for (size_t yy = 0; yy < kernel_info->font->height; yy++) {
        if (margin_x + window->position.y + tb_height + yy + y > window->position.y + window->size.y - 1 - margin_x)
            continue;
        for (size_t xx = 0; xx < kernel_info->font->width; xx++) {
            if (kernel_info->font) {
                if (margin_y + window->position.x + xx + x > window->position.x + window->size.x - 1 - margin_y)
                    continue;
                if (((*glyph & (0b10000000 >> ((xx + x) - x)))) > 0) {
                    plot_pixel_buffer(margin_x + window->position.x + x + xx, margin_y + window->position.y + tb_height + y + yy, color, back_buffer);
                }
            } else {
                if (margin_y + window->position.x + xx + x > window->position.x + window->size.x - 1 - margin_y)
                    continue;
                if (glyph[yy] & (1 << xx)) {
                    plot_pixel_buffer(margin_x + window->position.x + x + xx, margin_y + window->position.y + tb_height + y + yy, color, back_buffer);
                }
            }
        }
        if (kernel_info->font) {
            glyph++;
        }
    }
}
void window_render_text(window_t *window, char *s, int x, int y, uint32_t color) {
    char c;
    size_t x_backup = x;
    size_t y_backup = y;

    while ((c = *s++) != 0) {
        if (c == '\n') {
            y += kernel_info->font->height;
            continue;
        }
        if (c == '\r') {
            x = x_backup;
            continue;
        }

        window_render_char(window, c, x, y, color);
        x += 8;
    }
}

void window_render_text_margin(window_t *window, char *s, int x, int y, uint32_t color, int margin_x, int margin_y) {
    char c;
    size_t x_backup = x;
    size_t y_backup = y;

    while ((c = *s++) != 0) {
        if (c == '\n') {
            y += kernel_info->font->height;
            continue;
        }
        if (c == '\r') {
            x = x_backup;
            continue;
        }

        window_render_char_margin(window, c, x, y, color, margin_x, margin_y);
        x += 8;
    }
}

void window_render_rect(window_t *window, int x, int y, int width, int height, uint32_t color) {
    for (int yy = 0; yy < height; yy++) {
        if (window->position.y + tb_height + yy + y > window->position.y + window->size.y - 1)
            continue;

        for (int xx = 0; xx < width; xx++) {
            if (window->position.x + xx + x > window->position.x + window->size.x - 1)
                continue;

            plot_pixel_buffer(window->position.x + x + xx, window->position.y + y + tb_height + yy, color, back_buffer);
        }
    }
}

int window_init(vector2_t position, vector2_t size, char *title, void *update_function) {
    window_t window;
    window.position = position;
    window.size = size;

    if (window.size.x > 500)
        window.size.x = 500;

    if (window.size.y > 500)
        window.size.y = 500;

    window.title = title;
    window.background_color = LIGHTGRAY;
    window.title_bar_color = WHITE;
    window.update_function = update_function;
    window.active = true;
    windows[window_count++] = window;
    return window_count - 1;
}

void window_close(window_t *window) {
    window->active = false;
}

void window_open(window_t *window) {
    window->active = true;
}

void window_manager_handler() {
    window_t *foucsed_loop_window = NULL;
    for (int i = 0; i < window_count; i++) {
        if (!windows[i].active)
            continue;

        render_window(&windows[i]);

        vector2_t position = {
            .x = windows[i].position.x,
            .y = windows[i].position.y};

        vector2_t scale = {
            .x = windows[i].size.x,
            .y = tb_height};

        vector2_t scale_window = {
            .x = windows[i].size.x,
            .y = windows[i].size.y};

        if (collision(position, scale, mouse.position, (vector2_t){5, 5}) && mouse.left_clicked && currently_dragging == NULL) {
            currently_dragging = &windows[i].position;
        }

        if (collision(position, scale_window, mouse.position, (vector2_t){5, 5}) && mouse.left_clicked && currently_dragging == NULL) {
            focused_window = &windows[i];
            foucsed_loop_window = focused_window;
        }
    }

    if (!foucsed_loop_window && mouse.left_clicked)
        focused_window = NULL;

    if (!mouse.left_clicked) {
        currently_dragging = NULL;
    }

    if (currently_dragging != NULL) {
        currently_dragging->x = mouse.position.x;
        currently_dragging->y = mouse.position.y;
    }
}

void render_window(window_t *window) {
    int width = window->size.x;
    int height = window->size.y;
    uint32_t title_bar_color = display_color == WHITE ? (window->title_bar_color == WHITE ? BLACK : window->title_bar_color) : window->title_bar_color;

    // window itself
    for (int xx = 0; xx < width; xx++) {
        if (xx + window->position.x > kernel_info->video_mode_info.horizontal_resolution - 1)
            continue;
        for (int yy = 0; yy < height; yy++) {
            if (yy + window->position.y > kernel_info->video_mode_info.vertical_resolution - 1)
                continue;
            plot_pixel_buffer(xx + window->position.x, yy + window->position.y, window->background_color, back_buffer);
        }
    }
    // window itself

    // title bar
    for (int xx = 0; xx < width; xx++) {
        if (xx + window->position.x > kernel_info->video_mode_info.horizontal_resolution - 1)
            continue;
        for (int yy = 0; yy < tb_height; yy++) {
            if (yy + window->position.y > kernel_info->video_mode_info.vertical_resolution - 1)
                continue;
            plot_pixel_buffer(xx + window->position.x, yy + window->position.y, title_bar_color, back_buffer);
        }
    }

    int tb_center_x = (width / 2) - ((__strlen(window->title) * kernel_info->font->width) / 2);
    int tb_center_y = (tb_height / 2) - (kernel_info->font->height / 2);

    render_text_bb(window->title, window->position.x + tb_center_x, window->position.y + tb_center_y, 0xffffff - title_bar_color);
    // title bar

    // close button

    bool close_collision_check = collision(
        (vector2_t){window->position.x + window->size.x - 20, window->position.y + ((tb_height / 2) - (16 / 2))}, (vector2_t){16, 16},
        mouse.position, (vector2_t){5, 5});

    render_x_sprite(window->position.x + window->size.x - 20, window->position.y + ((tb_height / 2) - (16 / 2)), close_collision_check ? RED : 0xffffff - title_bar_color);

    if (close_collision_check && mouse.left_clicked) {
        window->active = false;
    }

    // close button

    // if (focused_window == window)
    ((void (*)())window->update_function)();
}

void button_init(button_t *button, vector2_t position, vector2_t size, char *label, uint32_t background_color, uint32_t hover_bg_color, uint32_t text_color) {
    button->position = position;
    button->size = size;
    button->label = label;
    button->hover_bg_color = hover_bg_color;
    button->background_color = background_color;
    button->text_color = text_color;
    button->on_click = NULL;
    button->on_hover = NULL;
    button->on_exit_hover = NULL;
}

void button_events(window_t *window, button_t *button) {
    if (collision((vector2_t){
                      .x = window->position.x + button->position.x,
                      .y = window->position.y + tb_height + button->position.y},
                  (vector2_t){.x = button->size.x, .y = button->size.y}, (vector2_t){.x = mouse.position.x, .y = mouse.position.y}, (vector2_t){.x = 16, .y = 16}) &&
        focused_window == window) {
        // hover
        if (button->on_hover != NULL)
            ((void (*)())button->on_hover)();

        if (mouse.left_clicked) {
            if (button->on_click != NULL)
                ((void (*)())button->on_click)();
        }
    } else {
        if (button->on_exit_hover != NULL)
            ((void (*)())button->on_exit_hover)();
    }
}

void window_render_button(window_t *window, button_t *button) {
    int x = button->position.x;
    int y = button->position.y;

    int center_x = (button->size.x / 2) - ((__strlen(button->label) * kernel_info->font->width) / 2);
    int center_y = (button->size.y / 2) - (kernel_info->font->height / 2);
    button_events(window, button);

    bool collision_check = collision((vector2_t){
                                         .x = window->position.x + button->position.x,
                                         .y = window->position.y + tb_height + button->position.y},
                                     (vector2_t){.x = button->size.x, .y = button->size.y}, (vector2_t){.x = mouse.position.x, .y = mouse.position.y}, (vector2_t){.x = 16, .y = 16});

    for (size_t yy = 0; yy < button->size.y; yy++) {
        if (window->position.y + tb_height + yy + y > window->position.y + window->size.y - 1)
            continue;
        for (size_t xx = 0; xx < button->size.x; xx++) {
            if (window->position.x + xx + x > window->position.x + window->size.x - 1)
                continue;

            plot_pixel_buffer(window->position.x + xx + x, window->position.y + tb_height + yy + y, collision_check ? button->hover_bg_color : button->background_color, back_buffer);
        }
    }

    render_text_bb(button->label, window->position.x + x + center_x, window->position.y + tb_height + y + center_y, button->text_color);
}