#include <interrupts/idt.h>
#include <interrupts/interrupts.h>
#include <paging/page_frame_allocator.h>
#include <port_io.h>

void idt_desc_entry_set_offset(idt_desc_entry *this, uint64_t offset) {
    this->offset0 = (uint16_t)(offset & 0x000000000000ffff);
    this->offset1 = (uint16_t)((offset & 0x00000000ffff0000) >> 16);
    this->offset2 = (uint16_t)((offset & 0xffffffff00000000) >> 32);
}

uint64_t idt_desc_entry_get_offset(idt_desc_entry *this) {
    uint64_t offset = 0;
    offset |= (uint64_t)this->offset0;
    offset |= (uint64_t)this->offset1 << 16;
    offset |= (uint64_t)this->offset2 << 32;
    return offset;
}

idtr_t idtr;

void set_idt_gate(void *handler, uint8_t entry_offset, uint8_t type_attr, uint8_t selector) {
    idt_desc_entry *interrupt = (idt_desc_entry *)(idtr.offset + entry_offset * sizeof(idt_desc_entry));
    idt_desc_entry_set_offset(interrupt, (uint64_t)handler);
    interrupt->type_attr = type_attr;
    interrupt->selector = selector;
}

void prepare_interrupts() {
    idtr.limit = 0x0FFF;
    idtr.offset = (uint64_t)pg_alloc_request_page(&global_allocator);

    set_idt_gate((void *)division_error_fault_handler, 0x0, IDT_TA_INTERRUPT_GATE, 0x08);
    set_idt_gate((void *)overflow_fault_handler, 0x4, IDT_TA_INTERRUPT_GATE, 0x08);
    set_idt_gate((void *)bound_range_exceeded_fault_handler, 0x5, IDT_TA_INTERRUPT_GATE, 0x08);
    set_idt_gate((void *)invalid_opcode_fault_handler, 0x6, IDT_TA_INTERRUPT_GATE, 0x08);
    set_idt_gate((void *)pagefault_handler, 0xE, IDT_TA_INTERRUPT_GATE, 0x08);
    set_idt_gate((void *)doublefault_handler, 0x8, IDT_TA_INTERRUPT_GATE, 0x08);
    set_idt_gate((void *)gpfault_handler, 0xD, IDT_TA_INTERRUPT_GATE, 0x08);
    set_idt_gate((void *)keyboard_int_handler, 0x21, IDT_TA_INTERRUPT_GATE, 0x08);
    set_idt_gate((void *)mouse_int_handler, 0x2C, IDT_TA_INTERRUPT_GATE, 0x08);
    set_idt_gate((void *)pit_int_handler, 0x20, IDT_TA_INTERRUPT_GATE, 0x08);

    asm("lidt %0"
        :
        : "m"(idtr));
    remap_pic();
    outb(PIC1_DATA, 0b11111000);
    outb(PIC2_DATA, 0b11101111);
    asm("sti");
}