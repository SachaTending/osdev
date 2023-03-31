#include "printf.h"
#include "libc.h"
void KernelStart();
extern "C" void _start() {
    printf("BOOTSTRAP: Calling constructors(needed by loggers)\n");
    callConstructors();
    printf("BOOTSTRAP: Starting.\n");
    KernelStart();
}