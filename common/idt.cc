#include "idt.h"
#include "common.h"
#include "printf.h"
#include "io.h"
#include "libc.h"

// This whole code copied from osdev wiki's interrupts tutorial

__attribute__((aligned(0x10))) 
static idt_entry_t idt[257]; // Create an array of IDT entries; aligned for performance

static idtr_t idtr;

extern "C" void* isr_stub_table[]; // some parts of code is diffier from original, because C++ xd

extern "C" void exception_handler() {
    printf("oops, exception\n");
    for (;;);
}

void kbd() {
    printf("kbd\n");
}

__attribute__((interrupt)) void generic(struct interrupt_frame *f) {
    printf("generic irq triggered\n");
    outb(0x20, 0x20);
}

static inline unsigned long read_cr2(void)
{
    unsigned long val;
    asm volatile ( "mov %%cr2, %0" : "=r"(val) );
    return val;
}

bool fixed = false;

handler handlers[16] = {0,0,0,0,0,0,0,0,0};

extern "C" void IntHandler(struct stackframe_t *stack) {
    if (stack->int_num < 32) {
        printf("Exception %u\n", stack->int_num);
        printf("ERR Code: %u\n", stack->err_code);
        if (stack->int_num == 14) {
            printf("cr2 = 0x%x\n", read_cr2());
        }
        printf("Happened at 0x%x\n", stack->rip);
        printf("Content: %x\n", (char *)stack->rdi);
        if (fixed == false) {
            printf("Trying to fix error...\n");
            char *target = (char *)stack->rdi;
            *target = 0x90;
            fixed = true;
        } else {
            printf("Error not fixed.\n");
            asm volatile ("nop");
            asm volatile ("cli");
            asm volatile ("hlt");
        }
        //asm volatile ("cli");
        //for (;;);
    } else {
        //printf("Interrupt!!! %u\n", stack->int_num);
        handler h = handlers[stack->int_num-32];
        if (h != NULL) {
            h(stack);
        } else {
            printf("Unknown int %u\n", stack->int_num);
        }
        outb(0x20, 0x20);
        if (stack->int_num > 40) {
            outb(0xA0, 0x20);
        }
    }
}

#define SELECTOR 0x28

void idt_set_desc(uint8_t vector, uint64_t isr, uint8_t flags) {
    uint64_t vec = isr;
    idt_entry_t* descriptor = &idt[vector];
    descriptor->isr_low        = vec & 0xFFFF;
    descriptor->kernel_cs      = SELECTOR;
    descriptor->ist            = 0;
    descriptor->attributes     = flags;
    descriptor->isr_mid        = (vec >> 16) & 0xFFFF;
    descriptor->isr_high       = (vec >> 32) & 0xFFFFFFFF;
    descriptor->reserved       = 0; // just for sure, this value is zero
}

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) {
    uint64_t vec = (uint64_t)isr;
    idt_entry_t* descriptor = &idt[vector];
    descriptor->isr_low        = vec & 0xFFFF;
    descriptor->kernel_cs      = SELECTOR;
    descriptor->ist            = 0;
    descriptor->attributes     = flags;
    descriptor->isr_mid        = (vec >> 16) & 0xFFFF;
    descriptor->isr_high       = (vec >> 32) & 0xFFFFFFFF;
    descriptor->reserved       = 0; // just for sure, this value is zero
}

void idt_set_descriptor(uint8_t vector, void (*isr)(), uint8_t flags) {
    uint64_t vec = (uint64_t)isr;
    idt_entry_t* descriptor = &idt[vector];
    descriptor->isr_low        = vec & 0xFFFF;
    descriptor->kernel_cs      = SELECTOR;
    descriptor->ist            = 0;
    descriptor->attributes     = flags;
    descriptor->isr_mid        = (vec >> 16) & 0xFFFF;
    descriptor->isr_high       = (vec >> 32) & 0xFFFFFFFF;
    descriptor->reserved       = 0; // just for sure, this value is zero
}

extern "C" uint64_t int_table[256];

uint8_t free_vector = 32;
uint8_t idt_alloc_vector() {
    if (free_vector == 0xf0) {
        panic("IDT: bruh, too many vectros\n");
    }
    return free_vector++;
}

void load_idt_pls() {
    __asm__ volatile ("lidt %0" :: "m"(idtr) : "memory"); // load the new IDT
    __asm__ volatile ("sti"); // set the interrupt flag
}
#define PIC1_CMD                    0x20
#define PIC1_DATA                   0x21
#define PIC2_CMD                    0xA0
#define PIC2_DATA                   0xA1
void IRQ_clear_mask(unsigned char IRQline) {
    uint16_t port;
    uint8_t value;

    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) & ~(1 << IRQline);
    outb(port, value);        
}

void idt_set_handl(int vector, handler h) {
    handlers[vector] = h;
}

void pic(struct stackframe_t *stack) {

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
    for (uint8_t vector=0; vector < 255; vector++) {
        //idt_set_descriptor(vector, (void *)generic, 0x8E);
        idt_set_desc(vector, int_table[vector], 0x8E);
    }
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x00);
    outb(0xA1, 0x00);

	outb(0xA1, 0xFF);
	outb(0x21, 0xFF);
    __asm__ volatile ("cli");
    __asm__ volatile ("lidt %0" :: "m"(idtr) : "memory"); // load the new IDT
    __asm__ volatile ("sti"); // set the interrupt flag
    int divisor = 1193180 / 100;
	outb(0x43, 0x36);
	outb(0x40, divisor & 0xff);
	outb(0x40, divisor >> 8);
    for (unsigned char i = 0;i<255;i++) {
        IRQ_clear_mask(i);
    }
    idt_set_handl(0, pic);
    idt_set_handl(43-32, pic);
    asm volatile ("hlt");
}