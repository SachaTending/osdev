#pragma once
#include "printf.h"

extern int tick;

#define LOG_START printf("[%d]",tick)
#define LOG_END printf("\n")

void log(const char *fmt);

class logger {
    public:
        logger(const char* prefix);
        void log(const char *fmt, ...);
    private:
        const char *log_name = "unknown";
};