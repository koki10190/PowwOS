#include <boot.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <uart.h>
#include <string.h>
#include <drivers/keyboard.h>
#include <graphics/graphics.h>
#include <display/display.h>
#include <stdlib.h>
#include <memory.h>
#include <shell/shell.h>
#include <gdt.h>
#include <interrupts/idt.h>
#include <paging/bitmap.h>
#include <paging/page_frame_allocator.h>
#include <paging/page_map_indexer.h>
#include <paging/paging.h>
#include <paging/page_table_manager.h>
#include <drivers/mouse.h>
#include <display/window.h>
#include <interrupts/interrupts.h>
#include <sprites.h>
#include <app/import_apps.h>
#include <pit/pit.h>
#include <desktop/desktop.h>
#include <ramfs/fs.h>
#include <stdio.h>
#include <cpuid.h>

#define MATCH_BG_COLOR display_color == WHITE ? BLACK : WHITE

static void draw_test_screen(Boot_Info *boot_info);

void kernel_main(Boot_Info *boot_info);

Boot_Info *kernel_info;
char *shell_name = "poww";
uint32_t display_color = 0x22bbdd;
uint32_t shell_name_color = LIGHTGREEN;

#define low_16(address) (uint16_t)((address)&0xFFFF)
#define high_16(address) (uint16_t)(((address) >> 16) & 0xFFFF)

uint8_t test_buffer[20];

extern uint64_t kernel_start;
extern uint64_t kernel_end;

page_frame_allocator_t global_allocator;

void test_window_update() {
}

void test_window_keyboard(uint8_t scancode) {
}

vector2_t *desktop_icon_dragging = NULL;
void desktop_icon_handler() {
    // Iterate through all the desktop icons
    for (int i = 0; i < desktop_icon_count; i++) {
        // Get the desktop icon
        desktop_icon_t *icon = &desktop_icons[i];

        // If the icon has a render function that renders the icon on screen, call it
        if (icon->icon_render_function)
            ((void (*)(int, int, uint32_t))icon->icon_render_function)(icon->position.x, icon->position.y, WHITE);

        // If the mouse is over the icon and left clicked, launch the application
        if (collision(icon->position, (vector2_t){32, 32}, mouse.position, (vector2_t){5, 5}) && mouse.left_clicked) {
            // If the icon has a valid init function, call the function
            if (icon->init_function)
                ((void (*)())icon->init_function)();
        }
        // If the mouse is over the icon and right clicked, start dragging the icon
        if (collision(icon->position, (vector2_t){32, 32}, mouse.position, (vector2_t){5, 5}) && mouse.right_clicked && desktop_icon_dragging == NULL) {
            desktop_icon_dragging = &icon->position;
        }
    }

    // If we're dragging the icon, then set the positions
    if (desktop_icon_dragging != NULL) {
        desktop_icon_dragging->x = mouse.position.x;
        desktop_icon_dragging->y = mouse.position.y;
    }

    // If the right click isn't clicked on anymore, then stop dragging the icon
    if (!mouse.right_clicked)
        desktop_icon_dragging = NULL;
}

void create_desktop_icons() {
    int icon_offset_increment = 45;
    int icon_offset = 0;

    // calculator
    new_desktop_icon((vector2_t){15 + icon_offset, 50}, calculator_init, render_calc_icon_sprite);
    icon_offset += icon_offset_increment;

    // terminal
    new_desktop_icon((vector2_t){15 + icon_offset, 50}, terminal_init, render_terminal_icon_sprite);
    icon_offset += icon_offset_increment;

    // background color selector
    new_desktop_icon((vector2_t){15 + icon_offset, 50}, bg_color_init, render_desktop_bg_sprite);
    icon_offset += icon_offset_increment;

    // file editor selector
    new_desktop_icon((vector2_t){15 + icon_offset, 50}, file_editor_init, render_file_editor_sprite);
    icon_offset += icon_offset_increment;

    new_desktop_icon((vector2_t){15 + icon_offset, 50}, paint_init, render_paint_sprite);
    icon_offset += icon_offset_increment;
}

void kernel_main(Boot_Info *boot_info) {
    kernel_info = boot_info;
    // Init UART
    uart_initialize();
    printf("Initializing Kernel...");
    printf("\nRSDP: ");
    printf(to_hstring64((uint64_t)boot_info->rsdp));
    printf("\n");

    graphics_init();    // init graphics
    set_divisor(65535); // pit

    /// Read through the memory map
    uint64_t mmap_entries = kernel_info->mmap_size / kernel_info->mmap_descriptor_size;
    pg_alloc_read_memory_map(&global_allocator, kernel_info->memory_map, kernel_info->mmap_size, kernel_info->mmap_descriptor_size);

    // Lock kernel
    uint64_t kernel_size = (uint64_t)&kernel_end - (uint64_t)&kernel_start;
    uint64_t kernel_pages = (uint64_t)kernel_size / 4096 + 1;
    pg_alloc_lock_pages(&global_allocator, &kernel_start, kernel_pages);

    // Lock framebuffer
    uint64_t framebuffer_base = (uint64_t)kernel_info->video_mode_info.framebuffer_pointer;
    uint64_t framebuffer_size = (uint64_t)kernel_info->video_mode_info.framebuffer_size + 0x1000;
    pg_alloc_lock_pages(&global_allocator, (void *)framebuffer_base, framebuffer_size / 0x1000);

    // Prepare ACPI
    prepare_acpi();

    // GDT and Interrupts Setup
    uart_puts("[POWW-KERNEL] Initialised.\n");
    gdt_descriptor __gdt_descriptor;
    __gdt_descriptor.size = sizeof(gdt) - 1;
    __gdt_descriptor.offset = (uint64_t)&default_gdt;
    gdt_init(&__gdt_descriptor);
    prepare_interrupts();

    // Init mouse driver
    init_mouse();

    set_divisor(65535); // Set PIT Divisor

    // Render the background color and create desktop icons
    render_background(display_color);
    create_desktop_icons();

    // Init shitty RamFS
    init_ramfs();

    int tick;
    while (1) {
        process_mouse_packet();

        // Swap from third buffer to the second
        swap_buffers();
        desktop_icon_handler();

        window_manager_handler();
        render_topbar();

        // Render the cursor pointer on screen
        clear_mouse_cursor(pointer_bitmap, mouse_position_old);
        render_mouse_cursor(pointer_bitmap, mouse.position, display_color == WHITE ? BLACK : WHITE);

        // Swap from second buffer to the front buffer
        __memcpy(kernel_info->video_mode_info.framebuffer_pointer, &back_buffer, 4096000);

        // Increment random seed
        seed(++rseed);
    }
}
