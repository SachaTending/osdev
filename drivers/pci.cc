#include "io.h"
#include "logger.h"
#include "stdint.h"
#include "pci.h"

logger pci_log("PCI");

typedef struct pci_trigger
{
    uint16_t venID;
    uint16_t devID;
    void (*trigger)(pci_dev_t dev);
} pci_trigger_t;

#define MAX_TRIGGERS 2048

pci_trigger_t triggers[MAX_TRIGGERS];

int trig_pos = 0;

uint32_t pciConfigReadW(pci_dev_t dev, uint8_t offset) {
    // Yes, this code copied from wiki.osdev.org
    uint32_t address;
    uint32_t lbus  = (uint32_t)dev.bus;
    uint32_t lslot = (uint32_t)dev.slot;
    uint32_t lfunc = (uint32_t)dev.func;
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

void pci_add_trig(void (*trigger)(pci_dev_t dev), uint16_t venID, uint16_t devID) {
    triggers[trig_pos] = {
        .venID = venID,
        .devID = devID,
        .trigger = trigger
    };
    trig_pos++;
    if (trig_pos > MAX_TRIGGERS) {
        pci_log.log("WARNING, trig_pos(%d) > MAX_TRIGGERS(%d), resetting trigger_position.\n", trig_pos, MAX_TRIGGERS);
        trig_pos = 0;
    }
}

void pci_init() {
    // This is bruteforce scan, because i dont want to recursive scan every bus
    pci_log.log("Scanning PCI Bus...\n");
    for (uint8_t bus = 0;bus<255;bus++) {
        for (uint8_t slot=0;slot<255;slot++) {
            for (uint8_t func=0;func<8;func++) {
                pci_dev_t dev = {
                    .bus = bus,
                    .slot = slot,
                    .func = func
                };
                uint16_t venID = pciConfigReadW(dev, 0);
                if(venID != 0xFFFF) // Imagine you created pci device with vendor id 0xFFFF
                {
                    uint16_t devID = pciConfigReadW(dev, 2);
                    uint8_t headerType = (uint8_t)pciConfigReadW(dev, 0xe);
                    const char *htype = "unknown";
                    if (headerType == 0x0) {
                        htype = "general device";
                    } else if (headerType == 0x80) {
                        htype = "pci to pci bridge";
                    } else if (headerType == 0x81) { // dont ask why, this is magic
                        htype = "pci to cardbus bridge";
                    }
                    pci_log.log("Found device, bus=%d slot=%d func=%d vendor=0x%x device=0x%x headerType=%s", bus, slot, func, venID, devID, htype);
                    if (headerType == 0x0) {
                        printf(" bar0=0x%x", pci_get_bar(dev, 0));
                    }
                    printf("\n");
                    for (int i=0;i<MAX_TRIGGERS;i++) {
                        if (triggers[i].devID == devID) {
                            if (triggers[i].venID == venID) {
                                pci_log.log("Detected trigger, starting...\n");
                                triggers[i].trigger(dev);
                            }
                        }
                    }
                }
            }
        }
    }
}

uint32_t pci_get_bar(pci_dev_t dev, int bar) {
    uint8_t bus = dev.bus;
    uint8_t slot = dev.slot;
    uint8_t func = dev.func;
    if (bar == 0) {
        //uint16_t bar_0 = pciConfigReadW(bus, slot, 0, 0x10);
        //uint16_t bar_1 = pciConfigReadW(bus, slot, 0, 0x12);
        //uint32_t out = (uint32_t)bar_0 << 16 | (uint32_t)bar_1;
        uint32_t address = (uint32_t)((bus << 16) | (slot << 11) |
              (func << 8) | (0x10 & 0xFC) | ((uint32_t)0x80000000));

        // Write out the address
        outl(0xCF8, address);
        uint32_t out = inl(0xCFC);
        return out;
    }
    return 0;
}