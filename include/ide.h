#pragma once
#include "stdint.h"

struct ide_dev {
    uint8_t bus, drive;

};

void ide_select_drive(uint8_t bus, uint8_t drive);
void ide_select_drive(ide_dev dev);
void ide_select_drive(ide_dev *dev);

void ide_poll(uint16_t io);
void ide_soft_reset(uint8_t io);
void ide_400ns(uint16_t io);
