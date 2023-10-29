#include <pit/pit.h>
#include <port_io.h>
#include <uart.h>
#include <string.h>

double time_since_boot = 0;
uint16_t divisor = 65535;
const uint64_t base_frequency = 119312;

void sleepd(double seconds) {
    double start_time = time_since_boot;
    uart_puts(__itoa(start_time));
    uart_puts(" ");
    uart_puts(__itoa(start_time + seconds));
    uart_puts("\n");

    while (time_since_boot < start_time + seconds) {
        asm("hlt");
    }
    uart_puts(__itoa(time_since_boot));
    uart_puts("\n");
}

void sleep(uint64_t miliseconds) {
    sleepd((double)miliseconds / 1000);
}

void set_divisor(uint16_t __divisor) {
    if (__divisor < 100)
        __divisor = 100;

    divisor = __divisor;
    outb(0x40, (uint8_t)(divisor & 0x00ff));
    io_wait();
    outb(0x40, (uint8_t)((divisor & 0xff00) >> 8));
}

uint16_t get_frequency() {
    return base_frequency / divisor;
}

void set_frequency(uint64_t frequency) {
    set_divisor(base_frequency / frequency);
}

void tick() {
    // uart_puts("tick interrupt");
    time_since_boot += 1 / (double)get_frequency();
}