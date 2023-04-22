#include "logger.h"
#include "printf.h"
#include "stdarg.h"
#include "stdint.h"

logger::logger(const char *prefix) {
    this->log_name = prefix;
    //printf("Logging: Logger %s ready.\n", prefix);
}
void lock();
void release();
void vsprintf(char * str, void (*putchar)(char), const char * format, va_list arg);
void print_adapter(char c);

void logger::log(const char *fmt, ...) {
    printf("[%d]", tick);printf(this->log_name);printf(": ");
    // The part of printf
    lock(); // because this is like printf, it acquires same lock as printf
    va_list ap;
    va_start(ap, fmt);
    vsprintf(NULL, print_adapter, fmt, ap);
    va_end(ap);
    release();
}

void log(const char *fmt) { // old logger
    LOG_START;
    printf("Kernel: ");
    printf(fmt);
}
