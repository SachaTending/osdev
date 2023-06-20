#include "libc.h"
#include "logger.h"
#include "common.h"
#include "pmm.h"

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
const char *FREE_magic = "FREE";

void pmm_init(uint64_t start, uint64_t len) {
    pmm.log("using base 0x%x length %u\n", start, len);
    base = start;
    length = len;
    current = base;
    end = base + len;
    pmm.log("end: 0x%x\n",end);
}

void free(void *addr) {
    entry *e = (entry *)(addr-sizeof(entry));
    //if (memcmp((const void *)(addr-sizeof(entry)), FREE_magic, sizeof(FREE_magic)) and memcmp((const void *)(addr-sizeof(entry)), USED_magic, sizeof(USED_magic))) pmm.log("WARN: Invalid magic: %s\n", e->magic); return;
    if (!(e->magic[0] == 'U' && e->magic[1] == 'S' && e->magic [2] == 'E' && e->magic[3] == 'D'))
    {
        if (!(e->magic[0] == 'F' && e->magic[1] == 'R' && e->magic[2] == 'E' && e->magic[3] == 'E')) pmm.log("WARN: Invalid magic: %s\n", e->magic); return;
    }
    if (e->magic[0] == 'U' && e->magic[1] == 'S' && e->magic [2] == 'E' && e->magic[3] == 'D') memcpy(e->magic, FREE_magic, 4);
    else if (e->magic[0] == 'F' && e->magic[1] == 'R' && e->magic[2] == 'E' && e->magic[3] == 'E') pmm.warning("Passed already free memory\n");
}

char *calloc(int a, int b) {
    return malloc(a*b);
}
bool is_free_found;
char *malloc(uint64_t len) {
    entry *e;
    if (end <= current) {

    } else {
        e = (entry *)current+1;
        memcpy(e->magic, USED_magic, 4);
        e->len = len;
        current += current+len+1;
    }
    if (end <= current && !is_free_found) {
        pmm.error("OUT OF MEMORY.\n");
        assert(end >= current);
    }
    return (char *)e+sizeof(entry);
}