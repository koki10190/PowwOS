#include <drivers/keyboard.h>
#include <uart.h>
#include <string.h>

char get_input_keycode() {
    char ch = 0;
    ch = inb(KEYBOARD_PORT);
    return ch;
}

char get_ascii_char(char keycode, int uppercase) {
    switch (keycode) {
    case 0x1e:
        return uppercase == 0 ? 'a' : 'A';
    case 0x30:
        return uppercase == 0 ? 'b' : 'B';
    case 0x2e:
        return uppercase == 0 ? 'c' : 'C';
    case 0x20:
        return uppercase == 0 ? 'd' : 'D';
    case 0x12:
        return uppercase == 0 ? 'e' : 'E';
    case 0x21:
        return uppercase == 0 ? 'f' : 'F';
    case 0x22:
        return uppercase == 0 ? 'g' : 'G';
    case 0x23:
        return uppercase == 0 ? 'h' : 'H';
    case 0x17:
        return uppercase == 0 ? 'i' : 'I';
    case 0x24:
        return uppercase == 0 ? 'j' : 'J';
    case 0x25:
        return uppercase == 0 ? 'k' : 'K';
    case 0x26:
        return uppercase == 0 ? 'l' : 'L';
    case 0x32:
        return uppercase == 0 ? 'm' : 'M';
    case 0x31:
        return uppercase == 0 ? 'n' : 'N';
    case 0x18:
        return uppercase == 0 ? 'o' : 'O';
    case 0x19:
        return uppercase == 0 ? 'p' : 'P';
    case 0x10:
        return uppercase == 0 ? 'q' : 'Q';
    case 0x13:
        return uppercase == 0 ? 'r' : 'R';
    case 0x1f:
        return uppercase == 0 ? 's' : 'S';
    case 0x14:
        return uppercase == 0 ? 't' : 'T';
    case 0x16:
        return uppercase == 0 ? 'u' : 'U';
    case 0x2f:
        return uppercase == 0 ? 'v' : 'V';
    case 0x11:
        return uppercase == 0 ? 'w' : 'W';
    case 0x2d:
        return uppercase == 0 ? 'x' : 'X';
    case 0x15:
        return uppercase == 0 ? 'y' : 'Y';
    case 0x2c:
        return uppercase == 0 ? 'z' : 'Z';
    case 0x02:
        return uppercase == 0 ? '1' : '!';
    case 0x03:
        return uppercase == 0 ? '2' : '"';
    case 0x04:
        return uppercase == 0 ? '3' : '3';
    case 0x05:
        return uppercase == 0 ? '4' : '$';
    case 0x06:
        return uppercase == 0 ? '5' : '%';
    case 0x07:
        return uppercase == 0 ? '6' : '^';
    case 0x08:
        return uppercase == 0 ? '7' : '&';
    case 0x09:
        return uppercase == 0 ? '8' : '*';
    case 0x0A:
        return uppercase == 0 ? '9' : '(';
    case 0x0B:
        return uppercase == 0 ? '0' : ')';
    case 0x0c:
        return uppercase == 0 ? '-' : '_';
    case 0x0d:
        return uppercase == 0 ? '=' : '+';
    case 0x1a:
        return uppercase == 0 ? '[' : '{';
    case 0x1b:
        return uppercase == 0 ? ']' : '}';
    case 0x27:
        return uppercase == 0 ? ';' : ':';
    case 0x28:
        return uppercase == 0 ? '\'' : '@';
    case 0x29:
        return uppercase == 0 ? '`' : '`';
    case 0x2b:
        return uppercase == 0 ? '\\' : '|';
    case 0x33:
        return uppercase == 0 ? ',' : '<';
    case 0x34:
        return uppercase == 0 ? '.' : '>';
    case 0x35:
        return uppercase == 0 ? '/' : '?';
    default:
        return uppercase == 0 ? '\0' : '\0';
    }
}