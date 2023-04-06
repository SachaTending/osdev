#include "initrd.h"
#include "libc.h"
#include "logger.h"
#include "limine.h"
#include "common.h"
#include "errors.h"

logger initrd("INITRD");

struct limine_module_request modules = {
    .id = LIMINE_MODULE_REQUEST,
    .revision = 1
};

ustar_t **initrd_file = 0;
int files = 0;
ustar_t *initrd_get_file(char *filename, ustar_t *out) {
    initrd.log("Initrd location: 0x%x\n", initrd_file);
    char *ptr = (char *)initrd_file;
    /*
    while (!memcmp(ptr+257, "ustar", 5))
    {
        if (!memcmp(ptr, filename, strlen(filename) + 1)) {
            int filesize = oct2bin((unsigned char *)ptr+0x7c, 11);
            out = (ustar_t *)ptr;
            return filesize;
        }
    }
    */
    for (int file=0;file<files;file++) {
        if (!memcmp(ptr, filename, strlen(filename))) {
            out = initrd_file[file];
            initrd.log("Found file\n");
            int filesize = oct2bin((unsigned char *)ptr+0x7c, 11);
            return initrd_file[file];
        }
    }
    //return -ENOFILE;
    
}

void initrd_init() {
    if (modules.response == NULL) {
        initrd.log("wtf, no modules?\n");
        assert(modules.response != NULL);
        return;
    }
    initrd.log("Info provided by limine: Internal module count: %d, module count: %u\n", modules.internal_module_count, modules.response->module_count); // crashes when trying to get module_count
    initrd.log("Parsing modules...\n");
    for (uint64_t i=0;i<modules.response->module_count;i++) {
        ustar_t *archive = (ustar_t *)modules.response->modules[i]->address;
        if (!memcmp((modules.response->modules[i]->address+257), "ustar", 5)) {
            initrd.log("Valid initrd at 0x%x\n", archive);
            //initrd.log("Filename: %s\n", archive->name);
            initrd_file = (ustar_t **)modules.response->modules[i]->address;
            break;
        }
    }
    initrd.log("Enumerating files...\n");
    for (uint64_t i=0;i<modules.response->module_count;i++) {
        ustar_t *archive = (ustar_t *)modules.response->modules[i]->address;
        if (!memcmp((modules.response->modules[i]->address+257), "ustar", 5)) {
            files++;
        }
    }
    initrd.log("Files: %d\n", files);
    initrd.log("GetFile()\n");
    ustar_t *out;
    ustar_t *s;
    char *ptr = (char *)initrd_file;
    for (int file=0;file<files;file++) {
        if (!memcmp(ptr, (const void *)"limine/LICENSE", strlen("limine/LICENSE"))) {
            out = initrd_file[file];
            initrd.log("Found file\n");
            initrd.log("Data: \n");
            printf((const char *)ptr+501);
            int filesize = oct2bin((unsigned char *)ptr+0x7c, 11);
            s = initrd_file[file];
        }
    }
    initrd.log("Returns 0x%x\n", s);
    initrd.log("Content: \n");
    //printf((const char *)out+501);
}