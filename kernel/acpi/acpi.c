#include "kernel/acpi/acpi.h"
#include "drivers/screen.h"
#include "libc/string.h"
#include "kernel/mem/vmm.h"

static void *lapic_base = 0;
static void *ioapic_base = 0;

static int memcmp(const void *s1, const void *s2, uint32_t n) {
    const uint8_t *p1 = s1, *p2 = s2;
    for (uint32_t i = 0; i < n; i++) if (p1[i] != p2[i]) return p1[i] - p2[i];
    return 0;
}

void acpi_init(void *rsdp_address) {
    acpi_rsdp_t *rsdp = (acpi_rsdp_t *)rsdp_address;
    if (memcmp(rsdp->signature, "RSD PTR ", 8) != 0) return;

    acpi_header_t *xsdt = (acpi_header_t *)rsdp->xsdt_address;
    if (memcmp(xsdt->signature, "XSDT", 4) != 0) return;

    uint32_t entries = (xsdt->length - sizeof(acpi_header_t)) / 8;
    uint64_t *table_ptrs = (uint64_t *)((uint64_t)xsdt + sizeof(acpi_header_t));

    for (uint32_t i = 0; i < entries; i++) {
        acpi_header_t *header = (acpi_header_t *)table_ptrs[i];
        if (memcmp(header->signature, "APIC", 4) == 0) {
            acpi_madt_t *madt = (acpi_madt_t *)header;
            lapic_base = (void *)(uint64_t)madt->lapic_address;
            vmm_map_page((uint64_t)lapic_base, (uint64_t)lapic_base, 3);

            // Parse MADT Records for I/O APIC
            uint8_t *ptr = (uint8_t *)madt + sizeof(acpi_madt_t);
            uint8_t *end = (uint8_t *)madt + madt->header.length;
            while (ptr < end) {
                uint8_t type = ptr[0];
                uint8_t length = ptr[1];
                if (type == 1) { // Type 1: I/O APIC
                    uint32_t addr = *(uint32_t *)(ptr + 4);
                    ioapic_base = (void *)(uint64_t)addr;
                    vmm_map_page((uint64_t)ioapic_base, (uint64_t)ioapic_base, 3);
                }
                ptr += length;
            }
            kprint("[ACPI] LAPIC and IOAPIC found and mapped.\n");
            return;
        }
    }
}

void *acpi_get_lapic_base() { return lapic_base; }
void *acpi_get_ioapic_base() { return ioapic_base; }
