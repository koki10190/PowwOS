#include <interrupts/interrupts.h>
#include <stdbool.h>
#include <uart.h>
#include <panic/panic.h>
#include <graphics/graphics.h>
#include <port_io.h>
#include <drivers/keyboard.h>
#include <shell/shell.h>
#include <display/display.h>
#include <boot.h>
#include <string.h>
#include <drivers/mouse.h>
#include <display/window.h>
#define MATCH_BG_COLOR display_color == WHITE ? BLACK : WHITE

__attribute__((interrupt)) void division_error_fault_handler(struct interrupt_frame_t *frame) {
    enter_panic_state("Division By Zero Detected!");

    while (true)
        ;
}

__attribute__((interrupt)) void bound_range_exceeded_fault_handler(struct interrupt_frame_t *frame) {
    enter_panic_state("Bound Range Exceeded!");

    while (true)
        ;
}
__attribute__((interrupt)) void invalid_opcode_fault_handler(struct interrupt_frame_t *frame) {
    enter_panic_state("Invalid Opcode!");

    while (true)
        ;
}
__attribute__((interrupt)) void overflow_fault_handler(struct interrupt_frame_t *frame) {
    enter_panic_state("Overflow Fault Detected!");

    while (true)
        ;
}

__attribute__((interrupt)) void pagefault_handler(struct interrupt_frame_t *frame) {
    enter_panic_state("Page Fault Detected!");

    while (true)
        ;
}

__attribute__((interrupt)) void doublefault_handler(struct interrupt_frame_t *frame) {
    enter_panic_state("Double Fault Detected!");

    while (true)
        ;
}
__attribute__((interrupt)) void gpfault_handler(struct interrupt_frame_t *frame) {
    enter_panic_state("General Protection Fault Detected!");

    while (true)
        ;
}

__attribute__((interrupt)) void keyboard_int_handler(struct interrupt_frame_t *frame) {
    uint8_t scan_code = inb(0x60);
    char ascii = get_ascii_char(scan_code, 0);

    bool shift_pressed = false;
    static char buffer[128] = "";
    static int buffer_index = 0;

    if (focused_window == NULL) {
        // switch (scan_code) {
        // case KEY_ENTER: {
        //     cmd_handler(buffer, false);
        //     render_shell_starter();
        //     for (int i = 0; i < 128; i++) {
        //         buffer[i] = '\0';
        //     }
        //     buffer_index = 0;
        //     break;
        // }
        // case KEY_BACKSPACE: {
        //     if (buffer_index != 0) {
        //         cursor_x -= 8;

        //         buffer[buffer_index--] = '\0';
        //         render_delchar(ascii, cursor_x, cursor_y, display_color);
        //     }
        //     break;
        // }
        // case KEY_SPACE: {
        //     buffer[buffer_index++] = ' ';
        //     char buf[2] = {' ', '\0'};
        //     print(buf, MATCH_BG_COLOR);
        //     break;
        // }
        // default: {
        //     if (ascii != '\0') {

        //         buffer[buffer_index++] = ascii;
        //         char buf[2] = {ascii, '\0'};
        //         print(buf, MATCH_BG_COLOR);
        //     }
        //     break;
        // }
        // }
    } else {
        if (focused_window->keyboard_interrupt_function != NULL) {
            ((void (*)(uint8_t))focused_window->keyboard_interrupt_function)(scan_code);
        }
    }
    pic_endmaster();
}

__attribute__((interrupt)) void mouse_int_handler(struct interrupt_frame_t *frame) {
    uint8_t mouse_data = inb(0x60);

    mouse_handle(mouse_data);

    pic_endslave();
}

void pic_endmaster() {
    outb(PIC1_COMMAND, PIC_EOI);
}
void pic_endslave() {
    outb(PIC2_COMMAND, PIC_EOI);
    outb(PIC1_COMMAND, PIC_EOI);
}

void remap_pic() {
    uint8_t a1, a2;

    a1 = inb(PIC1_DATA);
    io_wait();
    a2 = inb(PIC2_DATA);
    io_wait();

    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    outb(PIC1_DATA, 0x20);
    io_wait();
    outb(PIC2_DATA, 0x28);
    io_wait();

    outb(PIC1_DATA, 4);
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();

    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    outb(PIC1_DATA, a1);
    io_wait();
    outb(PIC2_DATA, a2);
}