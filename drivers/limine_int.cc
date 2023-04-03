/*
This file is interface to limine
*/

// Includes
#include "libc.h"
#include "limine.h"
#include "limine_int.h"
#include "common.h"

// Limine requests
// Limine will handle it, and writes a response

struct limine_terminal_request r = {
    .id = LIMINE_TERMINAL_REQUEST
};
struct limine_bootloader_info_request info_request = {
    .id = LIMINE_BOOTLOADER_INFO_REQUEST
};

struct limine_framebuffer_request framebuffer = {
    .id = LIMINE_FRAMEBUFFER_REQUEST
};

int x = 0;

// When you creating a multiboot kernel, you using a 0xB8000 for printing text, but in limine, you can just call limine's write function
void limine_write(const char *fmt) {
    r.response->write(r.response->terminals[0], fmt, strlen(fmt));
    //x++;
    //putpixel(x, 100, x);
}
limine_bootloader_info_response *limine_get_info() {
    return info_request.response;
}

void putpixel(int x,int y, int color) {
    struct limine_framebuffer *screen = framebuffer.response->framebuffers[0];
    if (screen->memory_model == LIMINE_FRAMEBUFFER_RGB) {
        unsigned where = x*screen->width + y*screen->pitch;
        ((unsigned char*)screen->address)[where] = (color >> screen->blue_mask_shift);              // BLUE
        ((unsigned char*)screen->address)[where + 1] = (color >> screen->green_mask_shift);   // GREEN
        ((unsigned char*)screen->address)[where + 2] = (color >> screen->red_mask_shift);  // RED
    }
}