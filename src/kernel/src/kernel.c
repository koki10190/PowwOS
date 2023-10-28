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

#define MATCH_BG_COLOR display_color == WHITE ? BLACK : WHITE

static void draw_test_screen(Boot_Info *boot_info);

void kernel_main(Boot_Info *boot_info);

Boot_Info *kernel_info;
char *shell_name = "poww";
uint32_t display_color = 0x22bbdd;
uint32_t shell_name_color = LIGHTGREEN;

#define low_16(address) (uint16_t)((address)&0xFFFF)
#define high_16(address) (uint16_t)(((address) >> 16) & 0xFFFF)

void keyboard_handler(struct Registers *regs) {
    char ch = inb(0x60);
    char keycode = get_ascii_char(ch, false);

    char kc[2] = {keycode, '\0'};
    uart_puts(kc);
}

uint8_t test_buffer[20];

extern uint64_t kernel_start;
extern uint64_t kernel_end;

page_frame_allocator_t global_allocator;

void test_window_update() {
}

void test_window_keyboard(uint8_t scancode) {
}

void desktop_icons() {
    render_calc_icon_sprite(15, 50, WHITE);
    render_terminal_icon_sprite(60, 50, WHITE);
    render_desktop_bg_sprite(60 + 45, 50, WHITE);

    if (collision((vector2_t){15, 50}, (vector2_t){32, 32}, mouse.position, (vector2_t){5, 5}) && mouse.left_clicked) {
        calculator_init();
    }
    if (collision((vector2_t){60, 50}, (vector2_t){32, 32}, mouse.position, (vector2_t){5, 5}) && mouse.left_clicked) {
        terminal_init();
    }
    if (collision((vector2_t){60 + 45, 50}, (vector2_t){32, 32}, mouse.position, (vector2_t){5, 5}) && mouse.left_clicked) {
        bg_color_init();
    }
}

void kernel_main(Boot_Info *boot_info) {
    kernel_info = boot_info;
    uart_initialize();
    uart_puts("Initializing Kernel...");
    uart_puts("\n");

    // paging and memory - start
    uint64_t mmap_entries = kernel_info->mmap_size / kernel_info->mmap_descriptor_size;
    pg_alloc_read_memory_map(&global_allocator, kernel_info->memory_map, kernel_info->mmap_size, kernel_info->mmap_descriptor_size);

    uint64_t kernel_size = (uint64_t)&kernel_end - (uint64_t)&kernel_start;
    uint64_t kernel_pages = (uint64_t)kernel_size / 4096 + 1;
    pg_alloc_lock_pages(&global_allocator, &kernel_start, kernel_pages);

    // page_table_t *pml4 = (page_table_t *)pg_alloc_request_page(&global_allocator);
    // m_memset(pml4, 0, 0x1000);

    // pt_manager page_table_manager;
    // pt_manager_construct(&page_table_manager, pml4);

    // uint64_t memsize = get_memory_size(kernel_info->memory_map, mmap_entries, kernel_info->mmap_descriptor_size);
    // uart_puts(__itoa(memsize));

    // for (uint64_t t = 0; t < memsize; t += 0x1000) {
    //     // uart_puts(__itoa(t));
    //     // uart_puts("\n");
    //     if (t < 131727360)
    //         pt_manager_map_memory(&page_table_manager, (void *)t, (void *)t);
    // }
    // uint64_t framebuffer_base = (uint64_t)kernel_info->video_mode_info.framebuffer_pointer;
    // uint64_t framebuffer_size = (uint64_t)kernel_info->video_mode_info.framebuffer_size + 0x1000;
    // pg_alloc_lock_pages(&global_allocator, (void *)framebuffer_base, framebuffer_size / 0x1000);

    // for (uint64_t t = framebuffer_base; t < framebuffer_base + framebuffer_size; t += 4096) {
    //     pt_manager_map_memory(&page_table_manager, (void *)t, (void *)t);
    // }
    // asm("mov %0, %%cr3"
    //     :
    //     : "r"(pml4));

    // paging and memory - end

    uart_puts("[POWW-KERNEL] Initialised.\n");
    gdt_descriptor __gdt_descriptor;
    __gdt_descriptor.size = sizeof(gdt) - 1;
    __gdt_descriptor.offset = (uint64_t)&default_gdt;
    gdt_init(&__gdt_descriptor);

    prepare_interrupts();
    init_mouse();
    outb(PIC1_DATA, 0b11111001);
    outb(PIC2_DATA, 0b11101111);
    STI();

    graphics_init();

    char keycode,
        ch = -1;
    bool shift_pressed = false;
    char buffer[128] = "";
    int buffer_index = 0;

    // setup render

    render_background(display_color);
    // print(shell_name, shell_name_color);
    // print(" ~ ", YELLOW);

    while (1) {
        process_mouse_packet();
        // print(buffer, MATCH_BG_COLOR)

        swap_buffers();
        desktop_icons();

        window_manager_handler();
        render_topbar();

        clear_mouse_cursor(pointer_bitmap, mouse_position_old);
        render_mouse_cursor(pointer_bitmap, mouse.position, display_color == WHITE ? BLACK : WHITE);
        __memcpy(kernel_info->video_mode_info.framebuffer_pointer, &back_buffer, 4096000);
        seed(++rseed);
    }
}
