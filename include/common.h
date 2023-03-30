#pragma once

#define cmab __attribute__((constructor)) // cmab - call me at boot
#include "stdint.h"
extern bool postcard_ready;
void postcard_send(uint8_t code);