#include <drivers/keyboard.h>
#include <uart.h>
#include <string.h>

bool shift_pressed = false;

char get_input_keycode() {
    char ch = 0;
    ch = inb(KEYBOARD_PORT);
    return ch;
}

char get_ascii_char(char keycode, int uppercase) {
    uart_puts(shift_pressed ? "true" : "false");
    switch (keycode) {
    case 0x1e:
        return shift_pressed == 0 ? 'a' : 'A';
    case 0x30:
        return shift_pressed == 0 ? 'b' : 'B';
    case 0x2e:
        return shift_pressed == 0 ? 'c' : 'C';
    case 0x20:
        return shift_pressed == 0 ? 'd' : 'D';
    case 0x12:
        return shift_pressed == 0 ? 'e' : 'E';
    case 0x21:
        return shift_pressed == 0 ? 'f' : 'F';
    case 0x22:
        return shift_pressed == 0 ? 'g' : 'G';
    case 0x23:
        return shift_pressed == 0 ? 'h' : 'H';
    case 0x17:
        return shift_pressed == 0 ? 'i' : 'I';
    case 0x24:
        return shift_pressed == 0 ? 'j' : 'J';
    case 0x25:
        return shift_pressed == 0 ? 'k' : 'K';
    case 0x26:
        return shift_pressed == 0 ? 'l' : 'L';
    case 0x32:
        return shift_pressed == 0 ? 'm' : 'M';
    case 0x31:
        return shift_pressed == 0 ? 'n' : 'N';
    case 0x18:
        return shift_pressed == 0 ? 'o' : 'O';
    case 0x19:
        return shift_pressed == 0 ? 'p' : 'P';
    case 0x10:
        return shift_pressed == 0 ? 'q' : 'Q';
    case 0x13:
        return shift_pressed == 0 ? 'r' : 'R';
    case 0x1f:
        return shift_pressed == 0 ? 's' : 'S';
    case 0x14:
        return shift_pressed == 0 ? 't' : 'T';
    case 0x16:
        return shift_pressed == 0 ? 'u' : 'U';
    case 0x2f:
        return shift_pressed == 0 ? 'v' : 'V';
    case 0x11:
        return shift_pressed == 0 ? 'w' : 'W';
    case 0x2d:
        return shift_pressed == 0 ? 'x' : 'X';
    case 0x15:
        return shift_pressed == 0 ? 'y' : 'Y';
    case 0x2c:
        return shift_pressed == 0 ? 'z' : 'Z';
    case 0x02:
        return shift_pressed == 0 ? '1' : '!';
    case 0x03:
        return shift_pressed == 0 ? '2' : '"';
    case 0x04:
        return shift_pressed == 0 ? '3' : '3';
    case 0x05:
        return shift_pressed == 0 ? '4' : '$';
    case 0x06:
        return shift_pressed == 0 ? '5' : '%';
    case 0x07:
        return shift_pressed == 0 ? '6' : '^';
    case 0x08:
        return shift_pressed == 0 ? '7' : '&';
    case 0x09:
        return shift_pressed == 0 ? '8' : '*';
    case 0x0A:
        return shift_pressed == 0 ? '9' : '(';
    case 0x0B:
        return shift_pressed == 0 ? '0' : ')';
    case 0x0c:
        return shift_pressed == 0 ? '-' : '_';
    case 0x0d:
        return shift_pressed == 0 ? '=' : '+';
    case 0x1a:
        return shift_pressed == 0 ? '[' : '{';
    case 0x1b:
        return shift_pressed == 0 ? ']' : '}';
    case 0x27:
        return shift_pressed == 0 ? ';' : ':';
    case 0x28:
        return shift_pressed == 0 ? '\'' : '@';
    case 0x29:
        return shift_pressed == 0 ? '`' : '`';
    case 0x2b:
        return shift_pressed == 0 ? '\\' : '|';
    case 0x33:
        return shift_pressed == 0 ? ',' : '<';
    case 0x34:
        return shift_pressed == 0 ? '.' : '>';
    case 0x35:
        return shift_pressed == 0 ? '/' : '?';
    default:
        return shift_pressed == 0 ? '\0' : '\0';
    }
}

bool scan_keycode(os_keycode keycode) {
    static os_keycode __scancode;
    outb(0x20, 0x20); // Send EOI
    __scancode = inb(0x60);

    if (__scancode == keycode)
        return true;
    else
        return false;
}