/**
 * @file port_io.c
 * @author ajxs
 * @date Aug 2019
 * @brief Port IO functionality.
 * Contains implementation for port IO.
 */

#include <port_io.h>

/**
 * inb
 */
uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0"
                 : "=a"(ret)
                 : "Nd"(port));
    return ret;
}

/**
 * outb
 */
void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1"
                 :
                 : "a"(val), "Nd"(port));
}

void io_wait() {
    asm volatile("outb %%al, $0x80"
                 :
                 : "a"(0));
}

void STI() {
    asm("sti");
}
void CLI() {
    asm("cli");
}