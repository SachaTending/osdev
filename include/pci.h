#pragma once
#include "stdint.h"

typedef struct pci_dev {
    uint8_t  bus;
    uint8_t  slot;
    uint8_t  func;
    uint16_t venID;
    uint16_t devID;
} pci_dev_t;

uint32_t pciConfigReadW(pci_dev_t dev, uint8_t offset);
void pci_init();
uint32_t pci_get_bar(pci_dev_t dev, int bar);
void pci_add_trig(void (*trigger)(pci_dev_t dev), uint16_t venID, uint16_t devID);