#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdbool.h>
#include <stdint.h>
#include <display/window.h>

typedef struct __termbuf_t {
    bool starter;
    char buf[1280];
    uint32_t color;
} termbuf_t;

extern window_t *terminal_window;
extern termbuf_t terminal_display_buffer[100];
extern int current_char_buffer_size;
extern int current_char_buffer_char_index;
extern bool terminal_skip_enter_nl;

void terminal_new_line();
void terminal_strcpy(char *src);
void terminal_print(char *buffer, uint32_t color);
void terminal_println(char *buffer, uint32_t color);
void terminal_clear_screen();

void terminal_init();

#endif