#include "printf.h"
#include "libc.h"
void KernelStart();
int init_serial();
void argparse();
extern "C" void _start() {
    printf("\x1b[32m");
    argparse();
    init_serial();
    printf("BOOTSTRAP: Calling constructors(needed by loggers)\n");
    callConstructors();
    printf("BOOTSTRAP: Starting.\n");
    KernelStart();
}