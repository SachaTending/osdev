#pragma once
#define cmab __attribute__((constructor)) // cmab - call me at boot
#include "stdint.h"
#include "limine.h"
extern bool postcard_ready;
void postcard_send(uint8_t code);

void panic(const char *reason);

void ASSERT(const char *data, bool result, const char * file, int line, const char * func);

#define assert(in) ASSERT(#in, in, __FILE__, __LINE__, __func__);

void putpixel(int x,int y, int color);

// features.cc
int catsay_main(const char *text);

// argparse.cc

namespace args {
    extern bool catsay;
    extern bool print_something;
};

extern limine_hhdm_request g_hhdm; // limine_int.cc

#define VM_HIGHER_HALF (g_hhdm.response->offset)

#define phys_to_virt(addr) (addr+VM_HIGHER_HALF)
#define virt_to_phys(addr) (addr-VM_HIGHER_HALF)