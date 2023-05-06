#pragma once
#include "stdint.h"

typedef union pci_bits {
    uint32_t bits;
    struct {
        uint32_t always_zero    : 2;
        uint32_t field_num      : 6;
        uint32_t function_num   : 3;
        uint32_t device_num     : 5;
        uint32_t bus_num        : 8;
        uint32_t reserved       : 7;
        uint32_t enable         : 1;
    };
} pci_bits_t;

typedef struct pci_dev {
    uint8_t  bus;
    uint8_t  slot;
    uint8_t  func;
    uint16_t venID;
    uint16_t devID;
    pci_bits_t bits;
} pci_dev_t;

uint32_t pciConfigReadW(pci_dev_t dev, uint8_t offset);
pci_dev_t pci_get_dev(uint32_t venID, uint32_t devID);
void pci_init();
uint32_t pci_get_bar(pci_dev_t dev, int bar);
void pci_add_trig(void (*trigger)(pci_dev_t dev), uint16_t venID, uint16_t devID);
void pci_dma_init(pci_dev_t dev);