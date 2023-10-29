#include <drivers/mouse.h>
#include <port_io.h>
#include <uart.h>
#include <string.h>
#include <stdint.h>
#include <boot.h>
#include <display/display.h>
#include <graphics/graphics.h>

uint8_t pointer_bitmap[] = {
    0b11111111,
    0b11100000,
    0b11111111,
    0b10000000,
    0b11111110,
    0b00000000,
    0b11111100,
    0b00000000,
    0b11111000,
    0b00000000,
    0b11110000,
    0b00000000,
    0b11100000,
    0b00000000,
    0b11000000,
    0b00000000,
    0b11000000,
    0b00000000,
    0b10000000,
    0b00000000,
    0b10000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
};

void mouse_wait() {
    uint64_t ticks = 100000;
    while (ticks--) {
        if ((inb(0x64) & 0b10) == 0) {
            return;
        }
    }
}

void mouse_wait_input() {
    uint64_t ticks = 100000;
    while (ticks--) {
        if (inb(0x64) & 0b1) {
            return;
        }
    }
}

void mouse_write(uint8_t value) {
    mouse_wait();
    outb(0x64, 0xD4);
    mouse_wait();
    outb(0x60, value);
}

uint8_t mouse_read() {
    mouse_wait_input();
    return inb(0x60);
}

void init_mouse() {
    outb(0x64, 0xA8);
    mouse_wait();
    outb(0x64, 0x20);
    mouse_wait_input();
    uint8_t status = inb(0x60);
    status |= 0b10;
    mouse_wait();
    outb(0x64, 0x60);
    mouse_wait();
    outb(0x60, status);

    mouse_write(0xF6);
    mouse_read();

    mouse_write(0xF4);
    mouse_read();
}

uint8_t mouse_cycle = 0;
uint8_t mouse_packet[4];
bool packet_ready = false;
mouse_t mouse;
vector2_t mouse_position_old;

void read_mouse_movement() {
    // mouse.movement.y
    uint8_t mouse_packet_copy[4];
    mouse_packet_copy[0] = mouse_packet[0];
    mouse_packet_copy[1] = mouse_packet[1];
    mouse_packet_copy[2] = mouse_packet[2];
    mouse_packet_copy[3] = mouse_packet[3];
    bool x_neg, y_neg, x_overflow, y_overflow;
    if (mouse_packet[0] & PS2XSign)
        x_neg = true;
    else
        x_neg = false;

    if (mouse_packet[0] & PS2YSign)
        y_neg = true;
    else
        y_neg = false;

    if (mouse_packet[0] & PS2XOverflow)
        x_overflow = true;
    else
        x_overflow = false;

    if (mouse_packet[0] & PS2YOverflow)
        y_overflow = true;
    else
        y_overflow = false;

    if (!x_neg) {
        mouse.movement.x = -mouse_packet_copy[1];
        if (x_overflow)
            mouse.movement.x = 255;
    } else {
        mouse_packet_copy[1] = 256 - mouse_packet_copy[1];
        mouse.movement.x = -mouse_packet_copy[1];
        if (x_overflow)
            mouse.movement.x = -255;
    }

    if (!y_neg) {
        mouse.movement.y = -mouse_packet_copy[2];
        if (y_overflow)
            mouse.movement.y = -255;
    } else {
        mouse_packet_copy[2] = 256 - mouse_packet_copy[2];
        mouse.movement.y = mouse_packet_copy[2];
        if (y_overflow) {
            mouse.movement.y = 255;
        }
    }
}

void process_mouse_packet() {
    if (!packet_ready)
        return;

    bool x_neg, y_neg, x_overflow, y_overflow;
    if (mouse_packet[0] & PS2XSign)
        x_neg = true;
    else
        x_neg = false;

    if (mouse_packet[0] & PS2YSign)
        y_neg = true;
    else
        y_neg = false;

    if (mouse_packet[0] & PS2XOverflow)
        x_overflow = true;
    else
        x_overflow = false;

    if (mouse_packet[0] & PS2YOverflow)
        y_overflow = true;
    else
        y_overflow = false;

    if (!x_neg) {
        mouse.position.x += mouse_packet[1];
        if (x_overflow)
            mouse.position.x += 255;
    } else {
        mouse_packet[1] = 256 - mouse_packet[1];
        mouse.position.x -= mouse_packet[1];
        if (x_overflow)
            mouse.position.x -= 255;
    }

    if (!y_neg) {
        mouse.position.y -= mouse_packet[2];
        if (y_overflow)
            mouse.position.y -= 255;
    } else {
        mouse_packet[2] = 256 - mouse_packet[2];
        mouse.position.y += mouse_packet[2];
        if (y_overflow) {
            mouse.position.y += 255;
            mouse.movement.y = 255;
        }
    }

    if (mouse.position.x < 0)
        mouse.position.x = 0;

    if (mouse.position.x > kernel_info->video_mode_info.horizontal_resolution - 1)
        mouse.position.x = kernel_info->video_mode_info.horizontal_resolution - 1;

    if (mouse.position.y < 0)
        mouse.position.y = 0;

    if (mouse.position.y > kernel_info->video_mode_info.vertical_resolution - 1)
        mouse.position.y = kernel_info->video_mode_info.vertical_resolution - 1;
    render_topbar();
    clear_mouse_cursor(pointer_bitmap, mouse_position_old);
    render_mouse_cursor(pointer_bitmap, mouse.position, display_color == WHITE ? BLACK : WHITE);
    mouse_position_old.x = mouse.position.x;
    mouse_position_old.y = mouse.position.y;

    if (mouse_packet[0] & PS2Leftbutton) {
        mouse.left_clicked = true;
    } else {
        mouse.left_clicked = false;
    }

    if (mouse_packet[0] & PS2Rightbutton) {
        mouse.right_clicked = true;
    } else {
        mouse.right_clicked = false;
    }

    if (mouse_packet[0] & PS2Middlebutton) {
        mouse.middle_clicked = true;
    } else {
        mouse.middle_clicked = false;
    }

    packet_ready = false;
}

void mouse_handle(uint8_t data) {
    switch (mouse_cycle) {
    case 0:
        if (packet_ready)
            break;

        if (data & 0xb00001000 == 0)
            break;

        mouse_packet[0] = data;
        mouse_cycle++;
        break;
    case 1:
        if (packet_ready)
            break;

        mouse_packet[1] = data;
        mouse_cycle++;
        break;
    case 2:
        if (packet_ready)
            break;

        mouse_packet[2] = data;
        packet_ready = true;
        mouse_cycle = 0;
        break;
    }
}

bool get_mouse_down_once(mouse_button_t button) {
    static bool left_clicked, right_clicked, middle_clicked;

    switch (button) {
    case LEFT_BTN: {
        if (mouse.left_clicked && !left_clicked) {
            left_clicked = true;
            return true;
        } else if (!mouse.left_clicked)
            left_clicked = false;
        return false;
    }
    case RIGHT_BTN:
        if (mouse.right_clicked && !right_clicked) {
            right_clicked = true;
            return true;
        } else if (!mouse.right_clicked)
            right_clicked = false;
        return false;
    case MIDDLE_BTN:
        if (mouse.middle_clicked && !middle_clicked) {
            middle_clicked = true;
            return true;
        } else if (!mouse.middle_clicked)
            middle_clicked = false;
        return false;
    }
}