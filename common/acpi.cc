#include "limine.h"
#include "logger.h"
#include "module.h"
#include "libc.h"
#include "common.h"
#include "acpi.h"

logger l("ACPI");

int redirect_count = 0;

struct RSDPDescriptor *rsdp;
struct RSDT           *rsdt;
struct FADT           *fadt;
struct MADTHeader     *madt;

extern uint32_t ioapic_addr;

struct limine_rsdp_request rsdp_request = {
    .id = LIMINE_RSDP_REQUEST
};

struct MADTIso redirects[256];

void *findFACP()
{
    int entries = (rsdt->h.Length - sizeof(rsdt->h)) / 4;

    for (int i = 0; i < entries; i++)
    {
        ACPISDTHeader *h = (ACPISDTHeader *) rsdt->OtherSDT[i];
        if (!memcmp(h->Signature, "FACP", 4))
            l.log("Found FACP\n");
            l.log("Signature: %s\n", h->Signature);
            return (void *) h;
    }

    // No FACP found
    return NULL;
}

void *findMADT()
{
    int entries = (rsdt->h.Length - sizeof(rsdt->h)) / 4;

    for (int i = 0; i < entries; i++)
    {
        ACPISDTHeader *h = (ACPISDTHeader *) rsdt->OtherSDT[i];
        if (h->Signature[0] == 'A') {
            l.log("Found MADT\n");
            l.log("Signature: %s\n", h->Signature);
            return (void *) h;
        }
    }

    // No MADT found
    return NULL;
}
void *
acpi_query(const char *query)
{
    size_t addr;
    struct ACPISDTHeader *current;
    bool is_valid;
    
    for (size_t i = 0; i < ((rsdt->h.Length - sizeof(rsdt->h)) / 4); ++i) {
        addr = (size_t)rsdt->OtherSDT[i] + VM_HIGHER_HALF;
        current = (struct ACPISDTHeader *)addr;
        if (memcmp(&(current->Signature), query, strlen(query)) == 0) {
            return (void *)current;
        }
    }
    
    return NULL;
}

void RedirectIRQ(uint8_t irq, uint8_t vec, uint32_t delivery);
void acpi_init() {
    l.log("RSDP: 0x%x\n", rsdp_request.response->address);  
    l.log("Revision: %u\n", rsdp_request.response->revision);
    rsdp = (struct RSDPDescriptor *)rsdp_request.response->address;
    l.log("OEM: %s\n", rsdp->OEMID);
    rsdt = (struct RSDT *)rsdp->RsdtAddress;
    l.log("RSDT: OEMID: %s\n", rsdt->h.OEMID);
    l.log("RSDT: Entries: %d\n", (rsdt->h.Length - sizeof(rsdt->h)) / 4);
    fadt = (struct FADT *)acpi_query("FACP");
    l.log("FADT Location: 0x%x\n", (uint64_t)fadt);
    l.log("FADT: DSDT Location: 0x%x\n", fadt->Dsdt);
    l.log("Searching for MADT...\n");
    madt = (struct MADTHeader *)findMADT();
    l.log("Parsing MADT...\n");
    for (uint8_t* madt_ptr = (uint8_t*)madt->entries_begin; (uint64_t)madt_ptr < (uint64_t)madt + madt->sdt.Length; madt_ptr += *(madt_ptr + 1))
	{
		switch (*(madt_ptr))
		{
		case 0:
			//l.log("Found LAPIC\n");
			break;
		case 1:
            ioapic_addr = (uint32_t)madt_ptr;
			//l.log("Found IOAPIC\n");
			break;
		case 2:
			redirects[redirect_count] = *(MADTIso*)madt_ptr;
			//l.log("Found IRQ redirect for IOAPIC (%d -> %d)\n", redirects[redirect_count].irq_source, redirects[redirect_count].gsi);
			redirect_count++;
			break;
		case 4:
			break;
		default:
			//l.log("MADT entry type %d\n", *(madt_ptr));
            break;
		}
	}
}

MODULE acpi_mod = {
    .name = {'A', 'C', 'P', 'I'},
    .mod_entry = acpi_init,
    .type = MOD_GENERIC
};