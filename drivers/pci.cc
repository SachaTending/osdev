#include "io.h"
#include "logger.h"
#include "stdint.h"
#include "pci.h"

logger pci_log("PCI");

uint16_t pciConfigReadW(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    // Yes, this code copied from wiki.osdev.org
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;

    // Create configuration address
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));

    // Write out the address
    outl(0xCF8, address);
    // Read in the data
    // (offset & 2) * 8) = 0 will choose the first word of the 32-bit register
    tmp = (uint16_t)((inl(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
    return tmp;
}

void pci_init() {
    // This is bruteforce scan, because i dont want to recursive scan every bus
    pci_log.log("Scanning PCI Bus...\n");
    for (uint8_t bus = 0;bus<255;bus++) {
        for (uint8_t slot=0;slot<255;slot++) {
            uint16_t venID = pciConfigReadW(bus, slot, 0, 0);
            if(venID != 0xFFFF) // Imagine you created pci device with vendor id 0xFFFF
            {
                uint16_t devID = pciConfigReadW(bus, slot, 0, 2);
                uint8_t headerType = (uint8_t)pciConfigReadW(bus, slot, 0, 0xe);
                const char *htype = "unknown";
                if (headerType == 0x0) {
                    htype = "general device";
                } else if (headerType == 0x80) {
                    htype = "pci to pci bridge";
                } else if (headerType == 0x81) { // dont ask why, this is magic
                    htype = "pci to cardbus bridge";
                }
                pci_log.log("Found device, bus=%d slot=%d vendor=0x%x device=0x%x headerType=%s", bus, slot, venID, devID, htype);
                if (headerType == 0x0) {
                    printf(" bar0=0x%x", pci_get_bar(bus, slot, 0));
                }
                printf("\n");
            }
        }
    }
}

uint32_t pci_get_bar(uint8_t bus, uint8_t slot, int bar) {
    if (bar == 0) {
        uint16_t bar_0 = pciConfigReadW(bus, slot, 0, 0x10);
        uint16_t bar_1 = pciConfigReadW(bus, slot, 0, 0x12);
        uint32_t out = (uint32_t)bar_0 << 16 | (uint32_t)bar_1;
        return out;
    }
    return 0;
}