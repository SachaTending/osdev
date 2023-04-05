#include "logger.h"
#include "module.h"
#include "stdint.h"

// This code copied from LimineOS(https://github.com/hello01-debug/LimineOS)

static logger l("I/O APIC");

uint32_t ioapic_addr = 0;

void ioapic_write(size_t reg, uint32_t data)
{
	*((uint32_t**)ioapic_addr)[0] = reg & 0xFF;
	*((uint32_t**)ioapic_addr)[16] = data;
}

void ioapic_write64(size_t reg, uint64_t data)
{
	uint32_t low = data & 0xFFFFFFFF;
    uint32_t high = (data >> 32) & 0xFFFFFFFF;

    ioapic_write(reg, low);
    ioapic_write(reg+1, high);
}
#define IO_APIC_RED_TABLE_ENT(x) (0x10 + 2 * x)
void RedirectIRQ(uint8_t irq, uint8_t vec, uint32_t delivery)
{
    l.log("RedirectIRQ(irq=%u vec=%u delivery=%u)\n", irq, vec, delivery);

	ioapic_write(IO_APIC_RED_TABLE_ENT(irq), vec | delivery);
    l.log("a\n");
    ioapic_write(IO_APIC_RED_TABLE_ENT(irq)+1, (vec | delivery) >> 32);
}

void ioapic_init() {
    l.log("placeholder\n");
}

MODULE ioapic_mod = {
    .name = {'I', '/', 'O', ' ', 'A', 'P', 'I', 'C'},
    .mod_entry = ioapic_init,
    .type = MOD_GENERIC
};