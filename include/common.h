#pragma once
#define cmab __attribute__((constructor)) // cmab - call me at boot
#include "stdint.h"
extern bool postcard_ready;
void postcard_send(uint8_t code);

void panic(const char *reason);

void ASSERT(const char *data, bool result, const char * file, int line, const char * func);

#define assert(in) ASSERT(#in, in, __FILE__, __LINE__, __func__);

void putpixel(int x,int y, int color);

// features.cc
int catsay_main(const char *text);