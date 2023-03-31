#include "limine.h"
#include "libc.h"
#include "limine_int.h"
#include "printf.h"
#include "logger.h"
#include "common.h"
#include "pci.h"


void end() {;
    log("Kernel halted, now.\n");
    asm volatile ("cli");
    for (;;) asm volatile ("hlt");
}

uint8_t bootstep = 0; /*
    Bootstep list:
    1.Init devices
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
    log("Kernel panic!!!, reason:");printf(reason);
    end();
}

void KernelStart()
{
    log("FloppaOS by TendingStream73\n");
    log("This kernel created because in old projects, im copied a lot of stuff, such as simple linker script\n");
    log("(hello to OSDev(Discord))\n");
    limine_bootloader_info_response *resp = limine_get_info();
    log("Im booted by: ");printf("%s version %s\n", resp->name, resp->version);
    inc_bootstep();
    log("Initializating...\n");
    pci_init();
    inc_bootstep();
    assert( 1 == 0 );
    end();
}
