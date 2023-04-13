#include "io.h"
#include "logger.h"
#include "module.h"
#include "ide.h"

logger ide("IDE");


#define ATA_PRIMARY_IO       0x1F0
#define ATA_SECONDARY_IO     0x170

#define ATA_PRIMARY_DCR_AS   0x3F6
#define ATA_SECONDARY_DCR_AS 0x376

#define ATA_MASTER           0x00
#define ATA_SLAVE            0x01

#define ATA_PRIMARY          0x00
#define ATA_SECONDARY        0x01

#define ATA_REG_DATA         0x00
#define ATA_REG_SECCOUNT0    0x02
#define ATA_REG_LBA0         0x03
#define ATA_REG_LBA1         0x04
#define ATA_REG_LBA2         0x05
#define ATA_REG_HDDEVSEL     0x06
#define ATA_REG_COMMAND      0x07
#define ATA_REG_STATUS       0x07

#define ATA_CMD_IDENTIFY     0xEC

#define ATA_SR_BSY           0x80
#define ATA_SR_ERR           0x01

#define ATA_IDENT_MODEL      54

uint16_t ide_buf[1024*1024];

bool ide_pm,ide_ps,ide_sm,ide_ss;

ide_dev devs[4] {
    {
        .bus = ATA_PRIMARY,
        .drive = ATA_MASTER
    },
    {
        .bus = ATA_PRIMARY,
        .drive = ATA_SLAVE
    },
    {
        .bus = ATA_SECONDARY,
        .drive = ATA_MASTER
    },
    {
        .bus = ATA_SECONDARY,
        .drive = ATA_SLAVE
    }
};

void ide_select_drive(uint8_t bus, uint8_t drive) {
    uint16_t port;
    uint16_t cmd;
    // Select port
    if (bus == ATA_PRIMARY)        port = ATA_PRIMARY_IO + ATA_REG_HDDEVSEL;
    else if (bus == ATA_SECONDARY) port = ATA_SECONDARY_IO + ATA_REG_HDDEVSEL;
    // Select command
    if (drive == ATA_MASTER)       cmd = 0xA0;
    else if (drive == ATA_SLAVE)   cmd = 0xB0;
    // And select drive
    outb(port, cmd);
}

uint8_t ide_identify(uint8_t bus, uint8_t drive) {
    uint16_t io = ATA_PRIMARY_IO;
    if (bus == ATA_SECONDARY) io = ATA_SECONDARY_IO;
    ide_select_drive(bus, drive);
    // Set registers to zero.
    outb(io+ATA_REG_SECCOUNT0, 0);
    outb(io+ATA_REG_LBA0, 0);
    outb(io+ATA_REG_LBA1, 0);
    outb(io+ATA_REG_LBA2, 0);
    // Send IDENTIFY.
    outb(io+ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
    uint8_t status = inb(io+ATA_REG_STATUS);
    if (status) {
        while((inb(io+ATA_REG_STATUS) & ATA_SR_BSY) != 0) {
            status = inb(io+ATA_REG_STATUS);
        }
        if (status & ATA_SR_BSY) {
            ide.log("%s %s has ERR set, disabled\n", bus?"Primary":"Secondary", drive?"master":"slave");
            return 0;
        }
        ide.log("%s %s online\n", bus?"Primary":"Secondary", drive?"master":"slave");
        for (int i=0;i<256;i++) {
            ide_buf[i*2] = inw(io+ATA_REG_DATA);
        }
        return 1;
    } else {
        ide.log("%s %s offline\n", bus?"Primary":"Secondary", drive?"master":"slave");
        return 0;
    }
}

void ide_init() {
    ide.log("Initializating...\n");
    for (uint8_t bus = 0;bus<2;bus++) {
        for (uint8_t drive = 0;drive<2;drive++) {
            uint8_t ret = ide_identify(bus, drive);
            if (ret == 1) {
                if(bus==ATA_PRIMARY){
                    if(drive==ATA_MASTER) ide_pm=true;
                    else if(drive==ATA_SLAVE) ide_ps=true;
                } else if (bus==ATA_SECONDARY) {
                    if(drive==ATA_MASTER) ide_sm=true;
                    else if(drive==ATA_SLAVE) ide_ss=true;
                }
            }
        }
    }
}

MODULE ide_mod = {
    .name = {'I', 'D', 'E'},
    .mod_entry = ide_init,
    .type = MOD_GENERIC
};