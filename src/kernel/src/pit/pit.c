#include <pit/pit.h>
#include <port_io.h>
#include <uart.h>

double time_since_boot = 0;
uint64_t base_frequency = 1193182;
uint16_t divisor = 65535;

void sleepd(double seconds) {
    double start_time = time_since_boot;
    while (time_since_boot < start_time + seconds) {
        asm("hlt");
    }
}

void sleep(uint64_t miliseconds) {
    sleepd((double)miliseconds / 1000);
}

void set_divisor(uint16_t __divisor) {
    if (divisor < 100)
        divisor = 100;

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
    uart_puts("tick interrupt");
    time_since_boot += 1 / (double)get_frequency();
}