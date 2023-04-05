#pragma once

enum mod_type {
    MOD_PCI = 1,
    MOD_GENERIC = 2
};

typedef void (*module_entry)();
struct module {
    char magic[3] = {'M', 'O', 'D'}; // Common magic for all modules
    char name[100]; // Name of module
    int struct_rev = 0; // Revision of structure
    module_entry mod_entry; // Function to call when scanning for modules
    int type = 2;
};

#define MODULE __attribute__((section(".mods_info"))) struct module