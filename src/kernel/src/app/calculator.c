#include <app/calculator.h>
#include <math/math.h>
#include <uart.h>
#include <string.h>
#include <memory.h>
#include <display/window.h>
#include <display/display.h>
#include <graphics/graphics.h>
#include <drivers/keyboard.h>

static window_t *calculator_window;
static char num_buffer1[30];
static char num_buffer1_index;
static char calc_operator = '\0';
int calc_num1 = -696969, calc_num2 = -696969;

void clear_number_buffer() {
    if (calc_operator == '\0' && calc_num1 == -696969) {
        calc_num1 = __strtoint(num_buffer1);
        reset_string_buffer(num_buffer1, 30);
        num_buffer1_index = 0;
    }
}

void plus_btn_click() {
    clear_number_buffer();
    calc_operator = '+';
}
void minus_btn_click() {
    clear_number_buffer();
    calc_operator = '-';
}
void divide_btn_click() {
    clear_number_buffer();
    calc_operator = '/';
}
void multiply_btn_click() {
    clear_number_buffer();
    calc_operator = '*';
}
void enter_btn_click() {
    // if operator is not selected
    if (calc_num1 == -696969)
        return;

    if (calc_num2 == -696969) {
        calc_num2 = __strtoint(num_buffer1);
        reset_string_buffer(num_buffer1, 30);
        num_buffer1_index = 0;
    }

    switch (calc_operator) {
    case '+':
        strcpy(num_buffer1, __itoa(calc_num1 + calc_num2));
        break;
    case '-':
        strcpy(num_buffer1, __itoa(calc_num1 - calc_num2));
        break;
    case '/':
        strcpy(num_buffer1, __itoa(calc_num1 / calc_num2));
        break;
    case '*':
        strcpy(num_buffer1, __itoa(calc_num1 * calc_num2));
        break;
    }
}

void reset_btn_click() {
    reset_string_buffer(num_buffer1, 30);
    num_buffer1_index = 0;

    calc_num1 = -696969;
    calc_num2 = -696969;
    calc_operator = '\0';
}

void calculator_update() {

    int window_width = calculator_window->size.x;
    int window_height = calculator_window->size.y;

    window_render_rect(calculator_window, 15, 15, window_width - 30, 20, CYAN);
    window_render_text(calculator_window, num_buffer1, 15 + 5, 15 + 4, BLACK);

    button_t plus_btn;
    button_init(&plus_btn, (vector2_t){15, 40}, (vector2_t){51, 51}, "+", calc_operator != '+' ? LIGHTBLUE : BLUE, BLUE, WHITE);

    button_t minus_btn;
    button_init(&minus_btn, (vector2_t){15 + (51 * 1 + (5 * 1)), 40}, (vector2_t){51, 51}, "-", calc_operator != '-' ? LIGHTBLUE : BLUE, BLUE, WHITE);

    button_t multiply_btn;
    button_init(&multiply_btn, (vector2_t){15 + (51 * 2 + (5 * 2)), 40}, (vector2_t){51, 51}, "X", calc_operator != '*' ? LIGHTBLUE : BLUE, BLUE, WHITE);

    button_t divide_btn;
    button_init(&divide_btn, (vector2_t){15 + (51 * 3 + (5 * 3)), 40}, (vector2_t){51, 51}, "/", calc_operator != '/' ? LIGHTBLUE : BLUE, BLUE, WHITE);
    button_t enter_btn;
    button_init(&enter_btn, (vector2_t){15, (80 + 15)}, (vector2_t){window_width - 30, 30}, "Enter", GREEN, LIGHTGREEN, WHITE);
    button_t reset_btn;
    button_init(&reset_btn, (vector2_t){15, (80 + 15) + 40}, (vector2_t){window_width - 30, 30}, "Reset", RED, LIGHTRED, WHITE);

    plus_btn.on_click = plus_btn_click;
    minus_btn.on_click = minus_btn_click;
    multiply_btn.on_click = multiply_btn_click;
    divide_btn.on_click = divide_btn_click;
    enter_btn.on_click = enter_btn_click;
    reset_btn.on_click = reset_btn_click;

    window_render_button(calculator_window, &plus_btn);
    window_render_button(calculator_window, &minus_btn);
    window_render_button(calculator_window, &multiply_btn);
    window_render_button(calculator_window, &divide_btn);
    window_render_button(calculator_window, &enter_btn);
    window_render_button(calculator_window, &reset_btn);
}

void calculator_keyboard_int(uint8_t scancode) {
    char ascii = get_ascii_char(scancode, 0);

    if (num_buffer1_index > 10)
        return;

    if (ascii == '1' || ascii == '2' || ascii == '3' || ascii == '4' || ascii == '5' || ascii == '6' || ascii == '7' || ascii == '8' || ascii == '9' || ascii == '0') {
        num_buffer1[num_buffer1_index++] = ascii;
    }

    if (scancode == KEY_BACKSPACE) {
        num_buffer1[num_buffer1_index--] = ' ';
    }
}

void calculator_init() {
    if (!calculator_window) {
        calculator_window = &windows[window_init((vector2_t){150, 150}, (vector2_t){250, 350}, "Calculator", calculator_update)];
        calculator_window->active = true;
        calculator_window->keyboard_interrupt_function = calculator_keyboard_int;
        calculator_window->background_color = 0x0A0A0A;
        focused_window = calculator_window;
    } else {
        calculator_window->active = true;
    }
}