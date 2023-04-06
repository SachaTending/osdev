#include "limine.h"
#include "libc.h"
#include "limine_int.h"
#include "printf.h"
#include "logger.h"
#include "common.h"
#include "pci.h"
#include "initrd.h"
#include "errors.h"
#include "module.h"
#include "io.h"

void end() {;
    log("Kernel halted, now.\n");
    asm volatile ("cli");
    for (;;) asm volatile ("hlt");
}
void load_idt_pls();

void halt() {
    // Same as end, but no cli
    log("Kernel halted.\n");
    load_idt_pls();
    for (;;) asm volatile ("hlt");
}

uint8_t bootstep = 0; /*
    Bootstep list:
    1.Init base
    2.end();
*/

int tick = 0;

void inc_bootstep() {
    bootstep++;
    LOG_START;printf("new bootstep: %u", bootstep);LOG_END;
    if (postcard_ready) {
        postcard_send(bootstep);
    }
}

void panic(const char *reason) {
    log("Kernel panic!!!, reason:");printf("%s", reason); // This is likely first pull request because Tigran#9100 told me to edit printf(reason); to printf("%s", reason);
    end();
}
void initrd_init();
void kbd_init();
void pic_init();

extern char mods_start;
extern char mods_end;

void start_modules(int type) {
    log("Starting modules with type ");printf("%d...\n", type);
    for (char *i = &mods_start;i != &mods_end;i++) {
        struct module *test = (struct module *)i;
        if (test->magic[0] == 'M' && test->magic[1] == 'O' && test->magic[2] == 'D' && test->type == type) {
            log("Detected module ");printf("%s\n", test->name);
            log("Starting it.\n");
            test->mod_entry();
        }
    }
}
#define ARGB(a, r, g, b) (a << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF)
struct limine_framebuffer_response *fb;

int c[4][3] = {
    {0, 1, 0},
    {0, 1, 0},
    {0, 0, 0},
    {0, 1, 1}
};

void rectangle() {
    int x = 200;
    int y = 200;
    int x2 = x+150;
    int y2=y+150;
    for (int x3=100;x3<x2;x3++) {
        for (int y3=100;y3<y2;y3++) {
            RGB_t color = {
                .r = 0,
                .g = x3,
                .b = y3
            };
            //printf("%d %d\n", x3, y3);
            putpixel(x3, y3, 0);
            putpixel(x3, y3, color);
        }
    }
    for (int i=0;i<4;i++) {
        for (int a=0;a<3;a++) {
            putpixel(i, a, c[i][a]);
        }
    }
}

void KernelStart()
{
    fb = limine_get_fb();
    log("\x1b[32m");
    log("FloppaOS by TendingStream73\n");
    log("This kernel created because in old projects, im copied a lot of stuff, such as simple linker script\n");
    log("(hello to OSDev(Discord))\n");
    log("kernel.c compiled at: ");printf("%s (mmm dd yyyy) %s (hh mm ss)\n", __DATE__, __TIME__);
    limine_bootloader_info_response *resp = limine_get_info();
    //for (;;);
    log("Im booted by: ");printf("%s version %s\n", resp->name, resp->version);
    inc_bootstep();
    log("Initializating...\n");
    pic_init();
    //kbd_init();
    start_modules(MOD_PCI);
    pci_init();
    start_modules(MOD_GENERIC);
    inc_bootstep();
    initrd_init();
    //printf("\e[2J\e[H");
    //rectangle();
    halt();
}
