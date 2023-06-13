#include "io.h"
#include "logger.h"
#include "common.h"
#include "module.h"
#include "idt.h"
#include "limine_int.h"

logger kbd("PS/2 KBD");

// Ports
#define DATA              0x60
#define CMD               0x64

// Returns from controller
#define ACK               0xFA

#define CNTRL_TEST_PASSED 0x55
#define CNTRL_TEST_FAILED 0xFC

#define PORT_TEST_PASSED  0x00

// Commands
#define TEST              0xAA
#define TEST_1_PORT       0xAB
#define TEST_2_PORT       0xA9

#define ENABLE_1_PORT     0xAE
#define ENABLE_2_PORT     0xA8

#define RESET_PORT        0xFF

void kbd_wait(int type=0) {
    int timeout=10000000;
    while (timeout--) {
        if (type == 0) {
            if ((inb(CMD) & 2) == 0) {
                return;
            }
        } else {
            if ((inb(CMD) & 1) == 1) {
                return;
            }
        }
    }
}

bool first_port_online  = false;
bool second_port_online = false;

void kbd_cmd(uint8_t cmd) {
    kbd_wait();
    outb(CMD, cmd);
}

void kbd_write(uint8_t cmd) {
    kbd_wait();
    outb(DATA, cmd);
}

void mouse_write(uint8_t data) {
    int time_out=100000;
    while (time_out--) {
        if((inb(0x64) & 2) != 0) continue;
    }
    outb(0x64, 0xD4);
    time_out=100000;
    while (time_out--) {
        if((inb(0x64) & 2) != 0) continue;
    }
    outb(0x60, data);
}

int mouse_x = 0, mouse_y = 0;

RGB_t color = {
    .a = 255,
    .r = 255,
    .g = 255,
    .b = 255
};

void mouse_int(stackframe_t *stack) {
    static uint8_t cycle = 0;
    static char bytes[3];
    int delay=1000;
    switch (cycle)
    {
        case 0:
            bytes[0] = inb(0x60);
            cycle++;
            while (delay--) {asm volatile("pause");}
            delay = 1000;
            break;
        case 1:
            bytes[1] = inb(0x60);
            cycle++;
            break;
        case 2:
            bytes[2] = inb(0x60);
            kbd.log("%d %d\n", (bytes[1]), (bytes[2]));
            cycle = 0;
            mouse_x = mouse_x + (bytes[1]);
            mouse_y = mouse_y - (bytes[2]);

            if (mouse_x < 0) mouse_x = 0;
            if (mouse_y < 0) mouse_y = 0;

            if (mouse_x > 100) mouse_x = 100;
            if (mouse_y > 100) mouse_y = 100;

            //kbd.log("mouse: all cycles passed\n");
            //kbd.log("x: %d y: %d\n", mouse_x, mouse_y);
            putpixel(mouse_x, mouse_y, color);
            break;

    }
}
bool kbd_in_init = false;
void kbd_enable() {
    //outb(CMD, 0xad);
    //outb(CMD, 0xa7);
    //kbd.log("Resetting ports...\n");
    //if (first_port_online) kbd_write(RESET_PORT);
    //if (second_port_online) kbd_cmd(0xD4);kbd_write(RESET_PORT);
    kbd.log("Enabling ports...\n");
    if (first_port_online) kbd_cmd(ENABLE_1_PORT);
    if (second_port_online) kbd_cmd(ENABLE_2_PORT);
    if (inb(0x60) != 0xfa) kbd.log("Mouse: controller returned value != 0xfa\n");
    kbd.log("Enabling interrupts...\n");
    kbd_cmd(0x20);
    while ((inb(0x64) & 1) == 0);
    uint8_t conf = inb(DATA);
    if (second_port_online) {
        kbd.log("DEBUG: Detected mouse, enabling interrupt.\n");
        conf = conf | 2;
    }
    conf |= (1 << 0) | (1 << 6);
    outb(CMD, 0x60);
    outb(0x60, conf);
    inb(0x60);
    
    mouse_write(0xF6);
    mouse_write(0xF4);
    kbd_in_init = false;
}
char kbdus[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */
    '9', '0', '-', '=', '\b',   /* Backspace */
    '\t',           /* Tab */
    'q', 'w', 'e', 'r', /* 19 */
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',       /* Enter key */
    0,          /* 29   - Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',   /* 39 */
    '\'', '`',   0,     /* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',         /* 49 */
    'm', ',', '.', '/',   0,                    /* Right shift */
    '*',
    0,  /* Alt */
    ' ',    /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
    '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
    '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};
char kbd_buf[1024];
int kbd_buf_p = 0;
#include "libc.h"
void kbd_handle() {
    uint8_t scancode = inb(0x60);
    if (scancode & 0x80) {
        return;
    }
    printf("%c", kbdus[scancode]);
    // Append char to buffer
    if (kbdus[scancode] == '\n') {
        printf("\n");
        kbd.log("Buffer = %s\n", kbd_buf);
        memset((void *)kbd_buf, '\0', 1024);
        kbd_buf_p = 0;
        return;
    }
    kbd_buf[kbd_buf_p] = kbdus[scancode];
    kbd_buf_p++;
}
void kbd_handl(stackframe_t *reg) {
    if (kbd_in_init);
    else kbd_handle();
}
void kbd_init() {
    kbd.log("Initializating...\n");
    for (int i=0;i<1024;i++) {
        kbd_buf[i] = '\0';
    }
    idt_set_handl(1, kbd_handl);
    idt_set_handl(12, mouse_int);
    // return;
    kbd_in_init = true;
    kbd_wait();
    outb(CMD, TEST);
    kbd_wait(1);
    uint8_t ret = inb(DATA);
    if (ret == CNTRL_TEST_PASSED) {
        kbd.log("PS/2 Controller: test passed\n");
        kbd.log("Testing ports...\n");
        kbd_wait();
        outb(CMD, TEST_1_PORT);
        kbd_wait(1);
        ret = inb(DATA);
        if (ret == PORT_TEST_PASSED) {
            kbd.log("Port 1 online\n");
            first_port_online = true;
        } else {
            kbd.log("Port 1 offline\n");
        }
        kbd_wait();
        outb(CMD, TEST_2_PORT);
        kbd_wait(1);
        ret = inb(DATA);
        if (ret == PORT_TEST_PASSED) {
            kbd.log("Port 2 online\n");
            second_port_online = true;
        } else {
            kbd.log("Port 2 offline\n");
        }
        kbd_enable();
    } else if (ret == CNTRL_TEST_FAILED) {
        kbd.log("PS/2 Controller: test failed\n");
    }
    
}

MODULE kbd_mod = {
    .name = {'k','b','d'},
    .mod_entry = kbd_init
};
