#pragma once
typedef void (*module_entry)();
struct module {
    char magic[3] = {'M', 'O', 'D'}; // Common magic for all modules
    char name[100]; // Name of module
    int struct_rev = 0; // Revision of structure
    module_entry mod_entry; // Function to call when scanning for modules
};

#define MODULE __attribute__((section(".mods_info"))) struct module