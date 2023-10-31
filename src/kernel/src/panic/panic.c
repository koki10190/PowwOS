#include <panic/panic.h>
#include <graphics/graphics.h>
#include <display/display.h>
#include <boot.h>
#include <string.h>
#include <memory.h>

void enter_panic_state(char *panic_reason) {
    render_background(RED);

    int screen_width = kernel_info->video_mode_info.horizontal_resolution;
    int screen_height = kernel_info->video_mode_info.vertical_resolution;

    int text_length = __strlen(panic_reason);

    int center_x = (screen_width / 2) - ((text_length * 8) / 2);
    int center_y = (screen_height / 2) - (kernel_info->font->height);

    render_text("what the fuck have you done.", 10, 10, BLACK);
    render_text(panic_reason, center_x, center_y, BLACK);
    swap_buffers();
    __memcpy(kernel_info->video_mode_info.framebuffer_pointer, &back_buffer, 4096000);
}