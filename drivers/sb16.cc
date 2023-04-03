#include "io.h"
#include "logger.h"
#include "module.h"
#include "libc.h"

// Ports
#define DSP_RESET      0x226
#define DSP_WRITE      0x22C
#define DSP_READ       0x22A

#define DSP_MIXER      0x224
#define DSP_MIXER_DATA 0x225

logger sb16("SB16");

int irq=0;

void sb16_init();

#define BUF_LEN 0x0FFF

unsigned **buf = (unsigned **)0x100450;

MODULE sb16_mod = {
    .name = {'s', 'b', '1', '6'},
    .mod_entry = sb16_init
};

void sb16_dma_init() {
    sb16.log("Preparing DMA for card...\n");
    outb(0xD4, 0x05);
    outb(0xD6, 0x58);
    outb(0x8B, 0x10);
    outb(0xC4, 0x50);
    outb(0xC4, 0x04);
    outb(0xC6, 0xFF);
    outb(0xC6, 0x0F);
    outb(0xD4, 1);
}

void sb16_init() {
    sb16.log("Initializating...\n");
    outb(DSP_RESET, 1);
    for (int i=0;i<1000000;i++) {asm volatile ("nop");};
    outb(DSP_RESET, 0);
    uint8_t ret = inb(DSP_READ);
    if (ret == 0xAA) {
        sb16.log("Found valid sb16 card\n");
        outb(DSP_MIXER, 0x80);
        ret = inb(DSP_MIXER_DATA);
        sb16.log("Card uses IRQ #");
        switch (ret)
        {
            case 0x01:
                printf("2\n");
                irq = 2;
                break;
            case 0x02:
                printf("5\n");
                irq = 5;
                break;
            case 0x04:
                printf("7\n");
                irq = 7;
                break;
            case 0x08:
                printf("10\n");
                irq = 10;
                break;
        }
        sb16_dma_init();
        sb16.log("Preparing card...\n");
        outb(DSP_WRITE, 0x40);
        outb(DSP_WRITE, 165);
        outb(DSP_WRITE, 0xC8);
        outb(DSP_WRITE, 0xB0);
        outb(DSP_WRITE, 0x00);
        outb(DSP_WRITE, 0xFE);
        outb(DSP_WRITE, 0x0F);
        sb16.log("Clearing buffer...\n");
        memset(buf, 0, BUF_LEN);
        sb16.log("Card is ready to play!\n");
        outb(DSP_WRITE, 0xD1);
        outb(DSP_WRITE, 0xD6);
    } else {
        sb16.log("Card not found.\n");
    }
}