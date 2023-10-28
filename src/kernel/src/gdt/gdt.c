#include <gdt.h>
#include <uart.h>
#include <gdt.h>

__attribute__((aligned(0x1000))) gdt default_gdt = {
    {0, 0, 0, 0x00, 0x00, 0}, // null,
    {0, 0, 0, 0x9a, 0xa0, 0}, // kernel code segment
    {0, 0, 0, 0x92, 0xa0, 0}, // kernel data segment
    {0, 0, 0, 0x00, 0x00, 0}, // user null
    {0, 0, 0, 0x9a, 0xa0, 0}, // user code segment
    {0, 0, 0, 0x92, 0xa0, 0}, // user data segment
};

void gdt_init(gdt_descriptor *gdt_descriptor) {
    load_gdt(gdt_descriptor);
    uart_puts("[POWW-KERNEL] GDT Initialized\r\n");
}