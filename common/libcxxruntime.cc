#include "printf.h"
#include "stdint.h"
#include "pmm.h"
namespace std
{
    typedef long unsigned int size_t;
}

void *operator new(std::size_t size) {
    printf("runtime: allocating %u bytes..\n", size);
    return malloc(size);
}

void operator delete(void *ptr) {
    free(ptr);
}
void operator delete(void *ptr, unsigned long sz) {
    free(ptr);
}