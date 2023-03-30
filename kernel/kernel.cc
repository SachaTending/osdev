#include "limine.h"
#include "libc.h"
#include "limine_int.h"
#include "printf.h"
#include "logger.h"
#include "common.h"

void halt_if_previous_method_didnt_work() {
    asm volatile ("hlt");
    halt_if_previous_method_didnt_work();
}

void end() {
    log("Kernel reached end, that sus\n");
    log("Kernel halted, now.\n");
    for (;;);
    log("wtf, kernel didn't halted, how?");
    halt_if_previous_method_didnt_work();
}

uint8_t bootstep = 0; /*
    Bootstep list:
    1.Call constructors
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
extern "C" void _start()
{
    log("FloppaOS by TendingStream73\n");
    log("This kernel created because in old projects, im copied a lot of stuff, such as simple linker script\n");
    log("(hello to OSDev(Discord))\n");
    limine_bootloader_info_response *resp = limine_get_info();
    log("Im booted by: ");printf("%s version %s\n", resp->name, resp->version);
    inc_bootstep();
    log("Calling constructors...\n");
    callConstructors();
    inc_bootstep();
    end();
}
