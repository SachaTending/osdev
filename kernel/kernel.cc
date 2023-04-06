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

void KernelStart()
{
    log("FloppaOS by TendingStream73\n");
    log("This kernel created because in old projects, im copied a lot of stuff, such as simple linker script\n");
    log("(hello to OSDev(Discord))\n");
    log("kernel.c compiled at: ");printf("%s (mmm dd yyyy) %s (hh mm ss)\n", __DATE__, __TIME__);
    limine_bootloader_info_response *resp = limine_get_info();
    log("Im booted by: ");printf("%s version %s\n", resp->name, resp->version);
    inc_bootstep();
    log("Initializating...\n");
    pic_init();
    //kbd_init();
    initrd_init();
    start_modules(MOD_PCI);
    pci_init();
    start_modules(MOD_GENERIC);
    inc_bootstep();
    halt();
}
