/*
This file is interface to limine
*/

// Includes
#include "libc.h"
#include "limine.h"
#include "limine_int.h"

// Limine requests
// Limine will handle it, and writes a response

struct limine_terminal_request r = {
    .id = LIMINE_TERMINAL_REQUEST
};
struct limine_bootloader_info_request info_request = {
    .id = LIMINE_BOOTLOADER_INFO_REQUEST
};


// When you creating a multiboot kernel, you using a 0xB8000 for printing text, but in limine, you can just call limine's write function
void limine_write(const char *fmt) {
    r.response->write(r.response->terminals[0], fmt, strlen(fmt));
}
limine_bootloader_info_response *limine_get_info() {
    return info_request.response;
}