#include "io.h"
#include "logger.h"
#include "libc.h"

static logger slog("Serial");

#define PORT 0x3f8          // COM1

int is_transmit_empty() {
    return inb(PORT + 5) & 0x20;
}
bool enabled = true;
void write_serial(char a) {
    if (!enabled) return;
    while (is_transmit_empty() == 0);

    outb(PORT,a);
}

void print_serial(const char *data) {
    size_t len = strlen(data);
    for (size_t i=0;i<len;i++) {
        write_serial(data[i]);
    }
}

int init_serial() {
    outb(PORT + 1, 0x00);    // Disable all interrupts
    outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(PORT + 1, 0x00);    //                  (hi byte)
    outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
    outb(PORT + 4, 0x1E);    // Set in loopback mode, test the serial chip
    outb(PORT + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)

    // Check if serial is faulty (i.e: not same byte as sent)
    if(inb(PORT + 0) != 0xAE) {
        return 1;
    }

    // If serial is not faulty set it in normal operation mode
    // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
    outb(PORT + 4, 0x0F);
    print_serial("FloppaOS: COM1 enabled.\n");
    enabled = true;
    return 0;
}