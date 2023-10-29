#include <app/terminal.h>
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
#include <shell/shell.h>

window_t *terminal_window;
termbuf_t terminal_display_buffer[100];
int current_char_buffer_size = 1;
int current_char_buffer_char_index = 0;

void terminal_new_line() {
    current_char_buffer_char_index = 0;
    current_char_buffer_size++;
    terminal_display_buffer[current_char_buffer_size - 1].starter = true;
}
void terminal_strcpy(char *src) {
    int len = __strlen(src);
    for (int i = 0; i < len; i++) {
        terminal_display_buffer[current_char_buffer_size - 1].buf[current_char_buffer_char_index++] = src[i];
    }
}

void terminal_print(char *buffer, uint32_t color) {
    terminal_display_buffer[current_char_buffer_size - 1].starter = false;
    terminal_display_buffer[current_char_buffer_size - 1].color = color;
    terminal_strcpy(buffer);
}

void terminal_println(char *buffer, uint32_t color) {
    terminal_display_buffer[current_char_buffer_size - 1].color = color;

    terminal_strcpy(buffer);
    terminal_new_line();
    terminal_display_buffer[current_char_buffer_size - 1].starter = false;
}

void terminal_os_string_to_color(uint32_t *color, char *buffer, bool bg_mode) {
    if (!strcmp(buffer, "blue")) {
        (*color) = BLUE;
    }
    if (!strcmp(buffer, "lblue")) {
        (*color) = LIGHTBLUE;
    }
    if (!strcmp(buffer, "green")) {
        (*color) = GREEN;
    }
    if (!strcmp(buffer, "cyan")) {
        (*color) = CYAN;
    }
    if (!strcmp(buffer, "red")) {
        (*color) = RED;
    }
    if (!strcmp(buffer, "magenta")) {
        (*color) = MAGENTA;
    }
    if (!strcmp(buffer, "pink") || !strcmp(buffer, "lmagenta")) {
        (*color) = LIGHTMAGENTA;
    }
    if (!strcmp(buffer, "brown")) {
        (*color) = BROWN;
    }
    if (!strcmp(buffer, "lgray")) {
        (*color) = LIGHTGRAY;
    }
    if (!strcmp(buffer, "dgray")) {
        (*color) = DARKGRAY;
    }
    if (!strcmp(buffer, "lgreen")) {
        (*color) = LIGHTGREEN;
    }
    if (!strcmp(buffer, "lcyan")) {
        (*color) = LIGHTCYAN;
    }
    if (!strcmp(buffer, "lred")) {
        (*color) = LIGHTRED;
    }
    if (!strcmp(buffer, "yellow")) {
        (*color) = YELLOW;
    }
    if (!strcmp(buffer, "white")) {
        (*color) = WHITE;
    }
    if (!strcmp(buffer, "black")) {
        (*color) = BLACK;
    }
}

void terminal_clear_screen() {
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 1280; j++) {
            terminal_display_buffer[i].buf[j] = '\0';
        }
        terminal_display_buffer[i].color = WHITE;
    }

    current_char_buffer_char_index = 0;
    current_char_buffer_size = 1;
}
bool terminal_skip_enter_nl = false;

void terminal_keyboard_int(uint8_t scancode) {
    char ascii = get_ascii_char(scancode, 0);

    switch (scancode) {
    case KEY_ENTER: {
        cmd_handler(terminal_display_buffer[current_char_buffer_size - 1].buf, true);
        if (!terminal_skip_enter_nl)
            terminal_new_line();
        break;
    }
    case KEY_BACKSPACE: {
        if (current_char_buffer_char_index > 0) {
            terminal_display_buffer[current_char_buffer_size - 1].buf[--current_char_buffer_char_index] = '\0';
        }
        break;
    }
    case KEY_SPACE: {
        if (current_char_buffer_char_index > 0) {
            terminal_display_buffer[current_char_buffer_size - 1].buf[current_char_buffer_char_index++] = ' ';
        }
        break;
    }
    default: {
        if (ascii != '\0') {
            terminal_display_buffer[current_char_buffer_size - 1].buf[current_char_buffer_char_index++] = ascii;
        }
    }
    }
    terminal_skip_enter_nl = false;
}

void terminal_update() {
    static int margin_x = 15, margin_y = 15;

    // window_render_text_margin(terminal_window, "poww ~", 0, 0, LIGHTGREEN, margin_x, margin_y);

    if (current_char_buffer_size > 17 * 2) {
        terminal_clear_screen();
    }

    int cursor_y = 0;
    for (int i = 0; i < current_char_buffer_size; i++) {
        char *buffer = terminal_display_buffer[i].buf;
        int name_len = __strlen(shell_name);
        int overall_len = name_len + 3;
        if (terminal_display_buffer[i].starter) {
            window_render_text_margin(terminal_window, shell_name, 0, cursor_y, shell_name_color, margin_x, margin_y);
            // window_render_text_margin(terminal_window, " ~ ", name_len * 8, cursor_y, YELLOW, margin_x, margin_y);
            window_render_text_margin(terminal_window, buffer, overall_len * 8, cursor_y, terminal_display_buffer[i].color, margin_x, margin_y);
        } else {
            window_render_text_margin(terminal_window, buffer, 0, cursor_y, terminal_display_buffer[i].color, margin_x, margin_y);
        }
        cursor_y += 10;
    }
}

static char *name = "Terminal";
void terminal_init() {
    static bool called_once;
    terminal_display_buffer[0].starter = true;

    for (int i = 0; i < 100; i++) {
        terminal_display_buffer[i].color = WHITE;
    }

    if (!terminal_window) {
        terminal_window = &windows[window_init((vector2_t){150, 150}, (vector2_t){500, 400}, name, terminal_update)];
        terminal_window->active = true;
        terminal_window->background_color = 0x0A0A0A;
        terminal_window->keyboard_interrupt_function = terminal_keyboard_int;
        focused_window = terminal_window;
    } else {
        terminal_window->active = true;
    }
}