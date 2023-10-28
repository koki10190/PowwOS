#include <graphics/graphics.h>
#include <memory.h>
#include <boot.h>
#include <display/display.h>

const uint8_t bitmap_1[128][8] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0000 (nul)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0001
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0002
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0003
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0004
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0005
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0006
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0007
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0008
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0009
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+000A
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+000B
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+000C
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+000D
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+000E
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+000F
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0010
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0011
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0012
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0013
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0014
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0015
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0016
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0017
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0018
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0019
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+001A
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+001B
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+001C
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+001D
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+001E
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+001F
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0020 (space)
    {0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00}, // U+0021 (!)
    {0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0022 (")
    {0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36, 0x00}, // U+0023 (#)
    {0x0C, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x0C, 0x00}, // U+0024 ($)
    {0x00, 0x63, 0x33, 0x18, 0x0C, 0x66, 0x63, 0x00}, // U+0025 (%)
    {0x1C, 0x36, 0x1C, 0x6E, 0x3B, 0x33, 0x6E, 0x00}, // U+0026 (&)
    {0x06, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0027 (')
    {0x18, 0x0C, 0x06, 0x06, 0x06, 0x0C, 0x18, 0x00}, // U+0028 (()
    {0x06, 0x0C, 0x18, 0x18, 0x18, 0x0C, 0x06, 0x00}, // U+0029 ())
    {0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00}, // U+002A (*)
    {0x00, 0x0C, 0x0C, 0x3F, 0x0C, 0x0C, 0x00, 0x00}, // U+002B (+)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x06}, // U+002C (,)
    {0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00}, // U+002D (-)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x00}, // U+002E (.)
    {0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00}, // U+002F (/)
    {0x3E, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x3E, 0x00}, // U+0030 (0)
    {0x0C, 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F, 0x00}, // U+0031 (1)
    {0x1E, 0x33, 0x30, 0x1C, 0x06, 0x33, 0x3F, 0x00}, // U+0032 (2)
    {0x1E, 0x33, 0x30, 0x1C, 0x30, 0x33, 0x1E, 0x00}, // U+0033 (3)
    {0x38, 0x3C, 0x36, 0x33, 0x7F, 0x30, 0x78, 0x00}, // U+0034 (4)
    {0x3F, 0x03, 0x1F, 0x30, 0x30, 0x33, 0x1E, 0x00}, // U+0035 (5)
    {0x1C, 0x06, 0x03, 0x1F, 0x33, 0x33, 0x1E, 0x00}, // U+0036 (6)
    {0x3F, 0x33, 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x00}, // U+0037 (7)
    {0x1E, 0x33, 0x33, 0x1E, 0x33, 0x33, 0x1E, 0x00}, // U+0038 (8)
    {0x1E, 0x33, 0x33, 0x3E, 0x30, 0x18, 0x0E, 0x00}, // U+0039 (9)
    {0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x00}, // U+003A (:)
    {0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x06}, // U+003B (;)
    {0x18, 0x0C, 0x06, 0x03, 0x06, 0x0C, 0x18, 0x00}, // U+003C (<)
    {0x00, 0x00, 0x3F, 0x00, 0x00, 0x3F, 0x00, 0x00}, // U+003D (=)
    {0x06, 0x0C, 0x18, 0x30, 0x18, 0x0C, 0x06, 0x00}, // U+003E (>)
    {0x1E, 0x33, 0x30, 0x18, 0x0C, 0x00, 0x0C, 0x00}, // U+003F (?)
    {0x3E, 0x63, 0x7B, 0x7B, 0x7B, 0x03, 0x1E, 0x00}, // U+0040 (@)
    {0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x00}, // U+0041 (A)
    {0x3F, 0x66, 0x66, 0x3E, 0x66, 0x66, 0x3F, 0x00}, // U+0042 (B)
    {0x3C, 0x66, 0x03, 0x03, 0x03, 0x66, 0x3C, 0x00}, // U+0043 (C)
    {0x1F, 0x36, 0x66, 0x66, 0x66, 0x36, 0x1F, 0x00}, // U+0044 (D)
    {0x7F, 0x46, 0x16, 0x1E, 0x16, 0x46, 0x7F, 0x00}, // U+0045 (E)
    {0x7F, 0x46, 0x16, 0x1E, 0x16, 0x06, 0x0F, 0x00}, // U+0046 (F)
    {0x3C, 0x66, 0x03, 0x03, 0x73, 0x66, 0x7C, 0x00}, // U+0047 (G)
    {0x33, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x33, 0x00}, // U+0048 (H)
    {0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}, // U+0049 (I)
    {0x78, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E, 0x00}, // U+004A (J)
    {0x67, 0x66, 0x36, 0x1E, 0x36, 0x66, 0x67, 0x00}, // U+004B (K)
    {0x0F, 0x06, 0x06, 0x06, 0x46, 0x66, 0x7F, 0x00}, // U+004C (L)
    {0x63, 0x77, 0x7F, 0x7F, 0x6B, 0x63, 0x63, 0x00}, // U+004D (M)
    {0x63, 0x67, 0x6F, 0x7B, 0x73, 0x63, 0x63, 0x00}, // U+004E (N)
    {0x1C, 0x36, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x00}, // U+004F (O)
    {0x3F, 0x66, 0x66, 0x3E, 0x06, 0x06, 0x0F, 0x00}, // U+0050 (P)
    {0x1E, 0x33, 0x33, 0x33, 0x3B, 0x1E, 0x38, 0x00}, // U+0051 (Q)
    {0x3F, 0x66, 0x66, 0x3E, 0x36, 0x66, 0x67, 0x00}, // U+0052 (R)
    {0x1E, 0x33, 0x07, 0x0E, 0x38, 0x33, 0x1E, 0x00}, // U+0053 (S)
    {0x3F, 0x2D, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}, // U+0054 (T)
    {0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3F, 0x00}, // U+0055 (U)
    {0x33, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00}, // U+0056 (V)
    {0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00}, // U+0057 (W)
    {0x63, 0x63, 0x36, 0x1C, 0x1C, 0x36, 0x63, 0x00}, // U+0058 (X)
    {0x33, 0x33, 0x33, 0x1E, 0x0C, 0x0C, 0x1E, 0x00}, // U+0059 (Y)
    {0x7F, 0x63, 0x31, 0x18, 0x4C, 0x66, 0x7F, 0x00}, // U+005A (Z)
    {0x1E, 0x06, 0x06, 0x06, 0x06, 0x06, 0x1E, 0x00}, // U+005B ([)
    {0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x40, 0x00}, // U+005C (\)
    {0x1E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1E, 0x00}, // U+005D (])
    {0x08, 0x1C, 0x36, 0x63, 0x00, 0x00, 0x00, 0x00}, // U+005E (^)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF}, // U+005F (_)
    {0x0C, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0060 (`)
    {0x00, 0x00, 0x1E, 0x30, 0x3E, 0x33, 0x6E, 0x00}, // U+0061 (a)
    {0x07, 0x06, 0x06, 0x3E, 0x66, 0x66, 0x3B, 0x00}, // U+0062 (b)
    {0x00, 0x00, 0x1E, 0x33, 0x03, 0x33, 0x1E, 0x00}, // U+0063 (c)
    {0x38, 0x30, 0x30, 0x3e, 0x33, 0x33, 0x6E, 0x00}, // U+0064 (d)
    {0x00, 0x00, 0x1E, 0x33, 0x3f, 0x03, 0x1E, 0x00}, // U+0065 (e)
    {0x1C, 0x36, 0x06, 0x0f, 0x06, 0x06, 0x0F, 0x00}, // U+0066 (f)
    {0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x1F}, // U+0067 (g)
    {0x07, 0x06, 0x36, 0x6E, 0x66, 0x66, 0x67, 0x00}, // U+0068 (h)
    {0x0C, 0x00, 0x0E, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}, // U+0069 (i)
    {0x30, 0x00, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E}, // U+006A (j)
    {0x07, 0x06, 0x66, 0x36, 0x1E, 0x36, 0x67, 0x00}, // U+006B (k)
    {0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}, // U+006C (l)
    {0x00, 0x00, 0x33, 0x7F, 0x7F, 0x6B, 0x63, 0x00}, // U+006D (m)
    {0x00, 0x00, 0x1F, 0x33, 0x33, 0x33, 0x33, 0x00}, // U+006E (n)
    {0x00, 0x00, 0x1E, 0x33, 0x33, 0x33, 0x1E, 0x00}, // U+006F (o)
    {0x00, 0x00, 0x3B, 0x66, 0x66, 0x3E, 0x06, 0x0F}, // U+0070 (p)
    {0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x78}, // U+0071 (q)
    {0x00, 0x00, 0x3B, 0x6E, 0x66, 0x06, 0x0F, 0x00}, // U+0072 (r)
    {0x00, 0x00, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x00}, // U+0073 (s)
    {0x08, 0x0C, 0x3E, 0x0C, 0x0C, 0x2C, 0x18, 0x00}, // U+0074 (t)
    {0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x6E, 0x00}, // U+0075 (u)
    {0x00, 0x00, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00}, // U+0076 (v)
    {0x00, 0x00, 0x63, 0x6B, 0x7F, 0x7F, 0x36, 0x00}, // U+0077 (w)
    {0x00, 0x00, 0x63, 0x36, 0x1C, 0x36, 0x63, 0x00}, // U+0078 (x)
    {0x00, 0x00, 0x33, 0x33, 0x33, 0x3E, 0x30, 0x1F}, // U+0079 (y)
    {0x00, 0x00, 0x3F, 0x19, 0x0C, 0x26, 0x3F, 0x00}, // U+007A (z)
    {0x38, 0x0C, 0x0C, 0x07, 0x0C, 0x0C, 0x38, 0x00}, // U+007B ({)
    {0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00}, // U+007C (|)
    {0x07, 0x0C, 0x0C, 0x38, 0x0C, 0x0C, 0x07, 0x00}, // U+007D (})
    {0x6E, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+007E (~)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}  // U+007F
};

uint32_t back_buffer[1280 * 1080];
uint32_t *overlay_buffer;
uint32_t mouse_cursor_buffer[16 * 16];
uint32_t mouse_cursor_buffer_after[16 * 16];

void graphics_init() {
    // uint32_t m_toswap_buffer[1280 * 1080];
    // toswap_buffer = m_toswap_buffer;
    // back_buffer = (void *)__malloc(4096000);
}

uint32_t get_pixel(int x, int y) {
    return back_buffer[x + (y * kernel_info->video_mode_info.horizontal_resolution)];
}

void plot_pixel(int x, int y, uint32_t color) {
    if (x > kernel_info->video_mode_info.horizontal_resolution - 1)
        return;
    if (y > kernel_info->video_mode_info.vertical_resolution - 1)
        return;
    kernel_info->video_mode_info.pixel_buffer[x + (y * kernel_info->video_mode_info.horizontal_resolution)] = color;
    // uint32_t *at;
    // at = kernel_info->video_mode_info.framebuffer_pointer + x;
    // at += (y * kernel_info->video_mode_info.pixels_per_scaline);
    // *at = color;
    // *((uint32_t *)(kernel_info->video_mode_info.framebuffer_pointer + 4 * kernel_info->video_mode_info.pixels_per_scaline * y + 4 * x)) = color;
}

void plot_pixel_buffer(int x, int y, uint32_t color, uint32_t *pixel_buffer) {
    if (x > kernel_info->video_mode_info.horizontal_resolution - 1)
        return;
    if (y > kernel_info->video_mode_info.vertical_resolution - 1)
        return;

    pixel_buffer[x + (y * kernel_info->video_mode_info.horizontal_resolution)] = color;
    // uint32_t *at;
    // at = kernel_info->video_mode_info.framebuffer_pointer + x;
    // at += (y * kernel_info->video_mode_info.pixels_per_scaline);
    // *at = color;
    // *((uint32_t *)(kernel_info->video_mode_info.framebuffer_pointer + 4 * kernel_info->video_mode_info.pixels_per_scaline * y + 4 * x)) = color;
}

void swap_buffers() {
    for (int yy = 0; yy < kernel_info->video_mode_info.vertical_resolution; yy++) {
        for (int xx = 0; xx < kernel_info->video_mode_info.horizontal_resolution; xx++) {
            back_buffer[xx + (yy * kernel_info->video_mode_info.horizontal_resolution)] = kernel_info->video_mode_info.pixel_buffer[xx + (yy * kernel_info->video_mode_info.horizontal_resolution)];
        }
    }
}

void render_char(char c, size_t x, size_t y, uint32_t color) {
    const uint8_t *glyph = bitmap_1[(size_t)c];

    for (size_t yy = 0; yy < 8; yy++) {
        for (size_t xx = 0; xx < 8; xx++) {
            if (glyph[yy] & (1 << xx)) {
                plot_pixel(x + xx, y + yy, color);
            }
        }
    }
}

void render_str(const char *s, size_t x, size_t y, uint32_t color) {
    char c;
    size_t x_backup = x;
    size_t y_backup = y;

    while ((c = *s++) != 0) {
        if (c == '\n') {
            y += 10;
            cursor_y += 10;
            continue;
        }
        if (c == '\r') {
            cursor_x = text_start_x;
            x = text_start_x;
            continue;
        }
        render_char(c, x, y, color);
        cursor_x += 8;
        x += 8;
    }
}

void render_text(const char *s, size_t x, size_t y, uint32_t color) {
    char c;
    size_t x_backup = x;
    size_t y_backup = y;

    while ((c = *s++) != 0) {
        if (c == '\n') {
            y += 10;
            cursor_y += 10;
            continue;
        }
        if (c == '\r') {
            x = x_backup;
            continue;
        }
        render_char(c, x, y, color);
        x += 8;
    }
}

void render_text_bb(const char *s, size_t x, size_t y, uint32_t color) {
    char c;
    size_t x_backup = x;
    size_t y_backup = y;

    while ((c = *s++) != 0) {
        if (c == '\n') {
            y += 10;
            cursor_y += 10;
            continue;
        }
        if (c == '\r') {
            x = x_backup;
            continue;
        }
        const uint8_t *glyph = bitmap_1[(size_t)c];

        for (size_t yy = 0; yy < 8; yy++) {
            for (size_t xx = 0; xx < 8; xx++) {
                if (glyph[yy] & (1 << xx)) {
                    plot_pixel_buffer(x + xx, y + yy, color, back_buffer);
                }
            }
        }
        x += 8;
    }
}

void render_delchar(char c, size_t x, size_t y, uint32_t color) {
    const uint8_t *glyph = bitmap_1[(size_t)c];

    for (size_t yy = 0; yy < 8; yy++) {
        for (size_t xx = 0; xx < 8; xx++) {
            // if (glyph[yy] & (1 << xx)) {
            plot_pixel(x + xx, y + yy, color);
            // }
        }
    }
}

bool pointer_drawn = false;

void clear_mouse_cursor(uint8_t *mouse_cursor, vector2_t pos) {
    // if (!pointer_drawn)
    //     return;
    // int screen_width = kernel_info->video_mode_info.horizontal_resolution;
    // int screen_height = kernel_info->video_mode_info.vertical_resolution;

    // int x_max = 16, y_max = 16;
    // int difference_x = screen_width - pos.x;
    // int difference_y = screen_width - pos.y;

    // if (difference_x < 16)
    //     x_max = difference_x;
    // if (difference_y < 16)
    //     y_max = difference_y;

    // for (int y = 0; y < y_max; y++) {
    //     for (int x = 0; x < x_max; x++) {
    //         int bit = y * 16;
    //         int byte = bit / 8;
    //         if ((mouse_cursor[byte] & (0b10000000 >> (x % 8)))) {
    //             if (get_pixel(pos.x + x, pos.y + y) == mouse_cursor_buffer_after[x + y * 16]) {

    //                 plot_pixel(pos.x + x, pos.y + y, mouse_cursor_buffer[x + y * 16]);
    //             }
    //         }
    //     }
    // }
}

uint32_t moues_pixel_buffer[16 * 16] = {
    0x000000, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800,
    0x000000, 0x000000, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800,
    0x000000, 0xffffff, 0x000000, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800,
    0x000000, 0xffffff, 0xffffff, 0x000000, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800,
    0x000000, 0xffffff, 0xffffff, 0xffffff, 0x000000, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800,
    0x000000, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0x000000, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800,
    0x000000, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0x000000, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800,
    0x000000, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0x000000, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800,
    0x000000, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0x000000, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800,
    0x000000, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0x000000, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800,
    0x000000, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800,
    0x000000, 0xffffff, 0xffffff, 0x000000, 0xffffff, 0xffffff, 0x000000, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800,
    0x000000, 0xffffff, 0x000000, 0xff5800, 0x000000, 0xffffff, 0x000000, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800,
    0x000000, 0x000000, 0xff5800, 0xff5800, 0x000000, 0xffffff, 0xffffff, 0x000000, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800,
    0x000000, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0x000000, 0xffffff, 0x000000, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800,
    0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0x000000, 0x000000, 0x000000, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800, 0xff5800};

void render_mouse_cursor(uint8_t *mouse_cursor, vector2_t pos, uint32_t color) {
    int screen_width = kernel_info->video_mode_info.horizontal_resolution;
    int screen_height = kernel_info->video_mode_info.vertical_resolution;

    int x_max = 16, y_max = 16;
    int difference_x = screen_width - pos.x;
    int difference_y = screen_width - pos.y;

    if (difference_x < 16)
        x_max = difference_x;
    if (difference_y < 16)
        y_max = difference_y;

    for (int y = 0; y < y_max; y++) {
        for (int x = 0; x < x_max; x++) {
            // int bit = y * 16 + x;
            // int byte = bit / 8;
            if ((moues_pixel_buffer[x + y * x_max] != 0xff5800)) {
                mouse_cursor_buffer[x + y * 16] = get_pixel(pos.x + x, pos.y + y);
                plot_pixel_buffer(pos.x + x, pos.y + y, moues_pixel_buffer[x + y * x_max] - get_pixel(pos.x + x, pos.y + y), back_buffer);
                mouse_cursor_buffer_after[x + y * 16] = get_pixel(pos.x + x, pos.y + y);
            }
        }
    }

    pointer_drawn = true;
}

bool collision(vector2_t pos1, vector2_t size1, vector2_t pos2, vector2_t size2) {
    return (
        pos1.x + size1.x >= pos2.x && // box1 right collides with box2 left
        pos2.x + size2.x >= pos1.x && // box2 right collides with box1 left
        pos1.y + size1.y >= pos2.y && // box1 bottom collides with box2 top
        pos2.y + size2.y >= pos1.y    // box1 top collides with box2 bottom
    );
}