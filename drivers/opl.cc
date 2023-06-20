// OPL driver
#include "stdint.h"
#include "io.h"
#include "opl.h"
#include "logger.h"

static logger l("OPL");

OPL::OPL(uint16_t port_base)
{
    this->port_base = port_base;
    // Detect OPL Chip
    this->Detect();
}
OPL::OPL() {

}
void OPL::WritePort(opl_port_t port, uint32_t value) {
    outb(this->port_base+port, value);
}

uint32_t OPL::ReadPort(opl_port_t port) {
    return inb(this->port_base+port);
}

void OPL::WriteReg(int reg, int val) {
    if (reg & 0x100) {
        this->WritePort(OPL_REGISTER_PORT_OPL3, reg);
    } else {
        this->WritePort(OPL_REGISTER_PORT, reg);
    }
    for (int i=0;i<6;i++) {
        if (this->init_stage_reg) {
            this->ReadPort(OPL_REGISTER_PORT);
        } else {
            this->ReadPort(OPL_DATA_PORT);
        }
    }
    this->WritePort(OPL_DATA_PORT, val);
    for (int i=0;i<24;i++) {
        this->ReadPort(OPL_REGISTER_PORT);
    }
}

void OPL::Detect() {
    int res1, res2;
    // Reset both timers
    this->WriteReg(OPL_REG_TIMER_CTRL, 0x60);
    // Enable interrupts
    this->WriteReg(OPL_REG_TIMER_CTRL, 0x80);
    // Read status
    res1 = this->ReadPort(OPL_REGISTER_PORT);
    // Set timer
    this->WriteReg(OPL_REG_TIMER1, 0xff);
    // Start timer 1
    this->WriteReg(OPL_REG_TIMER_CTRL, 0x21);
    // Wait for 80 microseconds
    for (int i=0;i<200;i++) {
        //l.log("wait\n");
        this-ReadPort(OPL_REGISTER_PORT);
    }
    for (int i=0;i<1000;i++) asm volatile ("pause");
    res2 = this->ReadPort(OPL_REGISTER_PORT);
    // Reset both timers
    this->WriteReg(OPL_REG_TIMER_CTRL, 0x60);
    // Enable interrupts
    this->WriteReg(OPL_REG_TIMER_CTRL, 0x80);
    if ((res1 & 0xe0) == 0x00 && (res2 & 0xe0) == 0xc0) {
        res1 = this->ReadPort(OPL_REGISTER_PORT);
        if (res1 == 0x00) {
            l.log("Detected OPL3 Chip on io base 0x%x\n", this->port_base);
        } else {
            l.log("Detected OPL2 Chip on io base 0x%x\n", this->port_base);
        }
    } else {
        l.error("No OPL Chip detected\n");
    }
}