#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI 0x20

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

struct interrupt_frame_t;
__attribute__((interrupt)) void division_error_fault_handler(struct interrupt_frame_t *frame);
__attribute__((interrupt)) void bound_range_exceeded_fault_handler(struct interrupt_frame_t *frame);
__attribute__((interrupt)) void invalid_opcode_fault_handler(struct interrupt_frame_t *frame);
__attribute__((interrupt)) void overflow_fault_handler(struct interrupt_frame_t *frame);

__attribute__((interrupt)) void pagefault_handler(struct interrupt_frame_t *frame);
__attribute__((interrupt)) void doublefault_handler(struct interrupt_frame_t *frame);
__attribute__((interrupt)) void gpfault_handler(struct interrupt_frame_t *frame);

// USB Devices
__attribute__((interrupt)) void keyboard_int_handler(struct interrupt_frame_t *frame);
__attribute__((interrupt)) void mouse_int_handler(struct interrupt_frame_t *frame);
__attribute__((interrupt)) void pit_int_handler(struct interrupt_frame_t *frame);

void remap_pic();
void pic_endmaster();
void pic_endslave();

#endif