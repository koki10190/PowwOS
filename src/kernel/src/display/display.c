#include <display/display.h>
#include <graphics/graphics.h>
#include <string.h>
#include <boot.h>
#include <time/rtc.h>
#include <drivers/mouse.h>
#include <display/window.h>

int glyph_offset_x = 8;
int glyph_offset_y = 8;
int text_start_x = 5, text_start_y = 25 + 10;
int cursor_x = 5, cursor_y = 25 + 10;

int center_x(int bg_width, int char_length) {
    return (bg_width / 2) - ((char_length * glyph_offset_x) / 2);
}

void render_topbar() {
    glyph_offset_x = kernel_info->font->width;
    glyph_offset_y = kernel_info->font->height;

    int width = kernel_info->video_mode_info.horizontal_resolution;
    int height = 25;
    uint32_t color_check = display_color == WHITE ? WHITE : BLACK;

    for (int xx = 0; xx < width; xx++) {
        for (int yy = 0; yy < height; yy++) {
            plot_pixel(xx, yy, display_color == WHITE ? BLACK : WHITE);
        }
    }

    int y_pos = (height / 2) - (glyph_offset_y / 2);

    render_text("PowwOS", 5, y_pos, color_check);
    int len = strlen("PowwOS") * glyph_offset_x;
    render_text("|", len + 8, y_pos, color_check);
    render_text(shell_name, len + (8 * 3), y_pos, color_check);

    if (focused_window != NULL) {
        render_text(focused_window->title, center_x(width, strlen(focused_window->title)), y_pos, color_check);
    }
    poww_time *time;
    get_time(time);
    // Time Render
    char *hour = __itoa(time->hour);
    char *minute = __itoa(time->min);

    int char_offset = 8, offset_from_right = 7;

    int hour_length = __strlen("20:00") * char_offset + offset_from_right;
    if (time->hour > 9)
        hour_length = __strlen("20:00") * char_offset + offset_from_right;
    else
        hour_length = __strlen("0:00") * char_offset + offset_from_right;
    int minute_length = __strlen("00") * char_offset + offset_from_right;
    int dot_length = __strlen("00:") * char_offset + offset_from_right;

    render_text(__itoa(time->hour), width - hour_length, y_pos, color_check);
    render_text(":", width - dot_length, y_pos, color_check);
    render_text(__itoa(time->min), width - minute_length, y_pos, color_check);
}

void render_background(uint32_t color) {
    for (int x = 0; x < kernel_info->video_mode_info.horizontal_resolution; x++) {
        for (int y = 0; y < kernel_info->video_mode_info.vertical_resolution; y++) {
            plot_pixel(x, y, color);
        }
    }

    render_mouse_cursor(pointer_bitmap, mouse.position, WHITE);
}

void print(char *buf, uint32_t color) {
    render_str(buf, cursor_x, cursor_y, color);
}
void println(char *buf, uint32_t color) {
    render_str(buf, cursor_x, cursor_y, color);
    render_str("\r\n", cursor_x, cursor_y, color);
}