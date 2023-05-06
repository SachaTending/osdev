#pragma once
#include "stdint.h"

typedef struct prdt {
	uint32_t buffer_phys;
	uint16_t transfer_size;
	uint16_t mark_end;
}__attribute__((packed)) prdt_t;

struct ide_dev {
    uint8_t bus, drive;
};

void ide_select_drive(uint8_t bus, uint8_t drive);
void ide_select_drive(ide_dev dev);
void ide_select_drive(ide_dev *dev);

void ide_poll(uint16_t io);
void ide_soft_reset(uint8_t io);
void ide_400ns(uint16_t io);

void ide_read_lba(void *buf, uint32_t lba, ide_dev dev);
