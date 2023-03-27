#include "io.h"
#include "common.h"
#include "logger.h"

bool postcard_ready = false;

void postcard_log(const char *fmt) {
    LOG_START;printf("POSTCard: ");printf(fmt);
}

void postcard_send(uint8_t code)
{
    outb(0x80, code); // osdev wiki says: this is 1-4 microsecond timer, used by bios to display postcode, so why i dont use it for displaying bootstep
    postcard_log("Setted POSTCode to ");printf("%u\n", code);
}

cmab void postcard_init() {
    postcard_log("POSTCard driver 0.1 by TendingStream73\n");
    postcard_send(1);
    postcard_ready = true;
}