#pragma once
#include "stdint.h"
uint16_t pciConfigReadW(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
void pci_init();
uint32_t pci_get_bar(uint8_t bus, uint8_t slot, int bar);