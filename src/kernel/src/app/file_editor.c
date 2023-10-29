#include <app/file_editor.h>
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
#include <ramfs/fs.h>

static window_t *fe_window;
static file_t *file_to_edit = NULL;
static char file_path[128] = "";
static char file_buffer[1024] = "";
static int file_buffer_index = 0;
static int file_path_index = 0;

void select_btn_click() {
    file_to_edit = ramfs_open(file_path);

    if (file_to_edit == NULL) {
        file_to_edit = ramfs_create(file_path, (uint8_t *)file_buffer);
        file_buffer_index = 0;
    } else {
        file_to_edit->deleted = false;
        strcpy(file_buffer, (char *)file_to_edit->buffer);
        file_buffer_index = __strlen(file_buffer);
    }
    if (file_to_edit == NULL)
        uart_puts("omfg");

    for (int i = 0; i < 128; i++)
        file_path[i] = '\0';

    file_path_index = 0;
}

void file_select_phase() {
    window_render_text(fe_window, "Type File Path:", 15, 20, LIGHTCYAN);
    if (!strcmp(file_path, "")) {
        window_render_text(fe_window, "...", 15, 35, LIGHTGRAY);
    } else {
        window_render_text(fe_window, file_path, 15, 32, WHITE);
    }

    button_t selected_btn;
    button_init(&selected_btn, (vector2_t){15, 50}, (vector2_t){__strlen("Type File Path:") * 8, 20}, "Select", LIGHTBLUE, BLUE, WHITE);
    selected_btn.on_click = select_btn_click;
    window_render_button(fe_window, &selected_btn);

    fe_window->size.y = 110;
}

void file_edit_phase() {
    fe_window->size.y = 400;

    window_render_text_margin(fe_window, (char *)file_buffer, 0, 0, WHITE, 15, 15);
}

void file_editor_update() {
    file_to_edit != NULL ? file_edit_phase() : file_select_phase();
}

void file_select_keyboard(uint8_t scancode) {
    char ascii = get_ascii_char(scancode, 0);

    if (ascii != '\0')
        file_path[file_path_index++] = ascii;

    if (scancode == KEY_BACKSPACE && file_path_index > 0)
        file_path[--file_path_index] = '\0';
}

void file_edit_keyboard(uint8_t scancode) {
    char ascii = get_ascii_char(scancode, 0);

    if (scancode == KEY_END) {
        char *buffer = "\0";
        strcpy(buffer, file_buffer);
        file_to_edit->buffer = (uint8_t *)buffer;
        file_to_edit = NULL;

        for (int i = 0; i < 1024; i++) {
            file_buffer[i] = '\0';
        }
        file_buffer_index = 0;
    } else if (scancode == KEY_ESC) {
        file_to_edit = NULL;
        for (int i = 0; i < 1024; i++) {
            file_buffer[i] = '\0';
        }
        file_buffer_index = 0;
    } else if (scancode == KEY_BACKSPACE && file_buffer_index > 0)
        file_buffer[--file_buffer_index] = '\0';
    else if (scancode == KEY_ENTER) {
        file_buffer[file_buffer_index++] = '\r';
        file_buffer[file_buffer_index++] = '\n';
    } else {
        if (ascii != '\0')
            file_buffer[file_buffer_index++] = ascii;
    }
}

void file_editor_keyboard_int(uint8_t scancode) {
    file_to_edit != NULL ? file_edit_keyboard(scancode) : file_select_keyboard(scancode);
}

void file_editor_init() {
    if (!fe_window) {
        fe_window = &windows[window_init((vector2_t){150, 150}, (vector2_t){500, 400}, "File Editor", file_editor_update)];
        fe_window->active = true;
        fe_window->background_color = 0x0A0A0A;
        fe_window->keyboard_interrupt_function = file_editor_keyboard_int;
        focused_window = fe_window;
    } else {
        fe_window->active = true;
    }
}
