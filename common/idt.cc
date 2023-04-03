#include "idt.h"
#include "common.h"
#include "printf.h"

// This whole code copied from osdev wiki's interrupts tutorial

__attribute__((aligned(0x10))) 
static idt_entry_t idt[256]; // Create an array of IDT entries; aligned for performance

static idtr_t idtr;

extern "C" void* isr_stub_table[]; // some parts of code is diffier from original, because C++ xd

extern "C" void exception_handler() {
    printf("oops, exception\n");
    for (;;);
}

void kbd() {
    printf("kbd\n");
}

#define SELECTOR 0x28

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) {
    idt_entry_t* descriptor = &idt[vector];
    descriptor->isr_low        = (uint64_t)isr & 0xFFFF;
    descriptor->kernel_cs      = SELECTOR;
    descriptor->ist            = 0;
    descriptor->attributes     = flags;
    descriptor->isr_mid        = ((uint64_t)isr >> 16) & 0xFFFF;
    descriptor->isr_high       = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
    descriptor->reserved       = 0; // just for sure, this value is zero
}

void idt_set_descriptor(uint8_t vector, void (*isr)(), uint8_t flags) {
    idt_entry_t* descriptor = &idt[vector];
    descriptor->isr_low        = (uint64_t)isr & 0xFFFF;
    descriptor->kernel_cs      = SELECTOR;
    descriptor->ist            = 0;
    descriptor->attributes     = flags;
    descriptor->isr_mid        = ((uint64_t)isr >> 16) & 0xFFFF;
    descriptor->isr_high       = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
    descriptor->reserved       = 0; // just for sure, this value is zero
}

// This part of code called from bootstrap
cmab void idt_init() {
    printf("IDT: Im doing stuff blah blah blah\n");
    idtr.base = (uint64_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt_entry_t) * 256 - 1;
    for (uint8_t vector = 0; vector < 32; vector++) {
        idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
        // vectors[vector] = true; // weird stuff from osdev wiki
    }
    idt_set_descriptor(32, kbd, 0x8E);
    idt_set_descriptor(33, kbd, 0x8E);
    __asm__ volatile ("lidt %0" : : "m"(idtr)); // load the new IDT
    __asm__ volatile ("sti"); // set the interrupt flag
    //int a = 0/0;
}