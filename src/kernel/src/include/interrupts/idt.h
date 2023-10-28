#ifndef IDT_H
#define IDT_H

#include <stdint.h>

#define IDT_TA_INTERRUPT_GATE 0b10001110
#define IDT_TA_CALL_GATE 0b10001100
#define IDT_TA_TRAP_GATE 0b10001111

typedef struct idt_desc_entry_t {
    uint16_t offset0;
    uint16_t selector;
    uint8_t ist;
    uint8_t type_attr;
    uint16_t offset1;
    uint32_t offset2;
    uint32_t ignore;
} idt_desc_entry;

void idt_desc_entry_set_offset(idt_desc_entry *this, uint64_t offset);
uint64_t idt_desc_entry_get_offset(idt_desc_entry *this);

typedef struct __idtr_t {
    uint16_t limit;
    uint16_t offset;
} __attribute__((packed)) idtr_t;
extern idtr_t idtr;

void prepare_interrupts();
void set_idt_gate(void *handler, uint8_t entry_offset, uint8_t type_attr, uint8_t selector);

#endif