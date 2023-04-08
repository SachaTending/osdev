#include "libc.h"
#include "logger.h"
#include "common.h"

logger pmm("PMM");

struct entry {
    char magic[5] = {'F', 'R', 'E', 'E'};
    uint64_t len;
};

uint64_t base;
uint64_t length;

uint64_t current;

uint64_t end;

const char *USED_magic = "USED";

void pmm_init(uint64_t start, uint64_t len) {
    pmm.log("using base 0x%x length %u\n", start, len);
    base = start;
    length = len;
    current = base;
    end = base + len;
    pmm.log("end: 0x%x\n",end);
}

char *malloc(uint64_t len) {
    if (end <= current) {
        pmm.log("ERR: OUT OF MEMORY.\n");
        assert(end >= current);
    }
    entry *e = (entry *)current+1;
    memcpy(e->magic, USED_magic, 4);
    e->len = len;
    current += current+len+1;
    return (char *)e+sizeof(entry);
}