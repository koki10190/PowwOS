#ifndef MOUSE_H
#define MOUSE_H
#include <stdint.h>
#include <stdbool.h>
#include <math/math.h>

#define PS2Leftbutton 0b00000001
#define PS2Middlebutton 0b00000010
#define PS2Rightbutton 0b00000100
#define PS2XSign 0b00010000
#define PS2YSign 0b00100000
#define PS2XOverflow 0b01000000
#define PS2YOverflow 0b10000000

void init_mouse();
void mouse_handle(uint8_t data);
void process_mouse_packet();

extern uint8_t pointer_bitmap[];

typedef struct __mouse_t {
    vector2_t position;
    bool left_clicked;
    bool right_clicked;
    bool middle_clicked;
} mouse_t;
extern mouse_t mouse;
extern vector2_t mouse_position_old;

#endif