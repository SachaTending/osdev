#pragma once
#include "printf.h"

extern int tick;

#define LOG_START printf("[%d]",tick)
#define LOG_END printf("\n")

void log(const char *fmt);