#include <stdio.h>
#include <uart.h>

void printf(char *s) {
    uart_puts(s);
}

void printf_ln(char *s) {
    uart_puts(s);
    uart_puts("\r\n");
}