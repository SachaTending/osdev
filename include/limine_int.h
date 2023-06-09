#pragma once
#include "limine.h"
void limine_write(const char *fmt);
limine_bootloader_info_response *limine_get_info();
limine_framebuffer_response *limine_get_fb();
limine_memmap_response *limine_get_memmap();
limine_kernel_file_response *limine_get_kernel_file();

typedef struct RGB
{
    int a=255;
    int r;
    int g;
    int b;
} RGB_t;
void putpixel(int x,int y, int color) ;
void putpixel(int x,int y, RGB_t color);