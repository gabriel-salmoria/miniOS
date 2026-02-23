#include <types.h>
#include "gdt.h"

typedef struct {
    uint16_t limit;
    uint16_t base_low;
    uint8_t  base_mid;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed)) gdt_entry_t;

typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) gdt_ptr_t;

// Define GDT in the data section
static gdt_entry_t gdt[3];
static gdt_ptr_t gdt_ptr;

void init_gdt() {
    // 1. Null Descriptor
    gdt[0] = (gdt_entry_t){0, 0, 0, 0, 0, 0};

    // 2. Kernel Code: Access 0x9A (Present, DPL 0, Code, Read), Granularity 0x20 (Long Mode)
    gdt[1].limit       = 0;
    gdt[1].base_low    = 0;
    gdt[1].base_mid    = 0;
    gdt[1].access      = 0x9A;
    gdt[1].granularity = 0x20;
    gdt[1].base_high   = 0;

    // 3. Kernel Data: Access 0x92 (Present, DPL 0, Data, Write), Granularity 0
    gdt[2].limit       = 0;
    gdt[2].base_low    = 0;
    gdt[2].base_mid    = 0;
    gdt[2].access      = 0x92;
    gdt[2].granularity = 0;
    gdt[2].base_high   = 0;

    gdt_ptr.limit = sizeof(gdt) - 1;
    gdt_ptr.base = (uint64_t)&gdt;

    // Load GDT
    __asm__ __volatile__("lgdt %0" : : "m"(gdt_ptr));

    // Reload segment registers using lretq for a 64-bit far return
    __asm__ __volatile__(
        "push $0x08\n"          // New Code Selector
        "lea 1f(%%rip), %%rax\n"
        "push %%rax\n"          // New RIP
        "lretq\n"               // Far Return to reload CS
        "1:\n"
        "mov $0x10, %%ax\n"     // New Data Selector (0x10)
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        "mov %%ax, %%ss\n"
        ::: "rax", "memory"
    );
}
