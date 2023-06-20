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

void logger::info(const char *fmt, ...) {
    printf("\x1b[97m[%d][%s][INFO]: ", tick, this->log_name);
    lock();
    va_list ap;
    va_start(ap, fmt);
    vsprintf(NULL, print_adapter, fmt, ap);
    va_end(ap);
    release();
}

void logger::warning(const char *fmt, ...) {
    printf("\x1b[93m[%d][%s][WARNING]: ", tick, this->log_name);
    lock();
    va_list ap;
    va_start(ap, fmt);
    vsprintf(NULL, print_adapter, fmt, ap);
    va_end(ap);
    release();
    printf("\x1b[97m"); // Restore color
}

void logger::success(const char *fmt, ...) {
    printf("\x1b[92m[%d][%s][SUCCESS]: ", tick, this->log_name);
    lock();
    va_list ap;
    va_start(ap, fmt);
    vsprintf(NULL, print_adapter, fmt, ap);
    va_end(ap);
    release();
    printf("\x1b[97m"); // Restore color
}

void logger::error(const char *fmt, ...) {
    printf("\x1b[91m[%d][%s][ERROR]: ", tick, this->log_name);
    lock();
    va_list ap;
    va_start(ap, fmt);
    vsprintf(NULL, print_adapter, fmt, ap);
    va_end(ap);
    release();
    printf("\x1b[97m"); // Restore color
}

void log(const char *fmt) { // old logger
    LOG_START;
    printf("[Kernel]: ");
    printf(fmt);
}
