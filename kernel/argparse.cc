#include "limine_int.h"
#include "printf.h"
#include "libc.h"

namespace args
{
    bool catsay=true;
    bool print_something=false;
}


void argparse() {
    limine_kernel_file_response *kern = limine_get_kernel_file();
    printf("args: %s\n", kern->kernel_file->cmdline);
    char *cmdl = kern->kernel_file->cmdline;
    while (*cmdl != '\0') {
        if (!memcmp(cmdl, "no-catsay", 10)) args::catsay=false;
        else if (!memcmp(cmdl, "print-something", 10)) args::print_something=true;
        cmdl++;
    }
}