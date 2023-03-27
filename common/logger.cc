#include "logger.h"
#include "printf.h"

void log(const char *fmt) {
    LOG_START;
    printf("Kernel: ");
    printf(fmt);
}
