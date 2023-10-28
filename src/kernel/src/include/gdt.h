#ifndef GDT_H
#define GDT_H

#include <stdint.h>
#include <stddef.h>

typedef struct gdt_descriptor_t {
    uint16_t size;
    uint64_t offset;
} __attribute__((packed)) gdt_descriptor;

typedef struct gdt_entry_t {
    uint16_t limit0;
    uint16_t base0;
    uint8_t base1;
    uint8_t access_byte;
    uint8_t limit1_flags;
    uint8_t base2;
} __attribute__((packed)) gdt_entry;

typedef struct gdt_t {
    gdt_entry null;        // 0x00
    gdt_entry kernel_code; // 0x08
    gdt_entry kernel_data; // 0x10
    gdt_entry user_null;
    gdt_entry user_code;
    gdt_entry user_data;
} __attribute__((packed)) __attribute__((aligned(0x1000))) gdt;

extern gdt default_gdt;

extern void load_gdt(gdt_descriptor *);

void gdt_init(gdt_descriptor *);

#endif