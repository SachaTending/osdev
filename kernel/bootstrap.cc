#include "printf.h"
#include "libc.h"
void KernelStart();
int init_serial();
extern "C" void _start() {
    printf("\x1b[32m");
    init_serial();
    printf("BOOTSTRAP: Calling constructors(needed by loggers)\n");
    callConstructors();
    printf("BOOTSTRAP: Starting.\n");
    KernelStart();
}