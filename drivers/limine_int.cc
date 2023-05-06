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

limine_memmap_request memmap {
    .id = LIMINE_MEMMAP_REQUEST
};

limine_kernel_file_request kern_file = {
    .id = LIMINE_KERNEL_FILE_REQUEST,
    .response = NULL
};

int x = 0;

// When you creating a multiboot kernel, you using a 0xB8000 for printing text, but in limine, you can just call limine's write function(its being  deprecated in 5.x tree :( ))
void limine_write(const char *fmt) {
    r.response->write(r.response->terminals[0], fmt, strlen(fmt));
    //x++;
    //putpixel(x, 100, x);
}
limine_bootloader_info_response *limine_get_info() {
    return info_request.response;
}

limine_framebuffer_response *limine_get_fb() {
    return framebuffer.response;
}

limine_memmap_response *limine_get_memmap() {
    return memmap.response;
}

limine_kernel_file_response *limine_get_kernel_file() {
    return kern_file.response;
}

void putpixel(int x,int y, int color) {
    struct limine_framebuffer *screen = framebuffer.response->framebuffers[0];
    if (screen->memory_model == LIMINE_FRAMEBUFFER_RGB) {
        uint32_t *where = (uint32_t *)(screen->address);
        size_t row = (y * screen->pitch) / 4;
        where[row + x] = color;
    }
}

#define ARGB(a, r, g, b) (a << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF)

void putpixel(int x,int y, RGB_t color) {
    struct limine_framebuffer *screen = framebuffer.response->framebuffers[0];
    if (screen->memory_model == LIMINE_FRAMEBUFFER_RGB) {
        uint32_t *where = (uint32_t *)(screen->address);
        size_t row = (y * screen->pitch) / 4;
        where[row + x] = ARGB(color.a, color.r, color.g, color.b);
    }
}