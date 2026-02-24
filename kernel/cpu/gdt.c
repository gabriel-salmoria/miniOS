#include "kernel/cpu/gdt.h"
#include "libc/string.h"

static uint64_t gdt[7];
static tss_t tss;

struct gdt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

static struct gdt_ptr gdtr;

static void set_gdt_entry(int index, uint64_t base, uint32_t limit, uint8_t access, uint8_t flags) {
    gdt[index] = (limit & 0xFFFF) | ((base & 0xFFFFFF) << 16) |
                 ((uint64_t)access << 40) | (((uint64_t)limit & 0xF0000) << 32) |
                 ((uint64_t)flags << 52) | ((base & 0xFF000000) << 32);
}

void init_gdt() {
    memset(&tss, 0, sizeof(tss_t));
    tss.iopb_offset = sizeof(tss_t);

    gdt[0] = 0;
    set_gdt_entry(1, 0, 0xFFFFF, 0x9A, 0xA); // KERNEL_CS
    set_gdt_entry(2, 0, 0xFFFFF, 0x92, 0xC); // KERNEL_DS
    set_gdt_entry(3, 0, 0xFFFFF, 0xFA, 0xA); // USER_CS (DPL 3)
    set_gdt_entry(4, 0, 0xFFFFF, 0xF2, 0xC); // USER_DS (DPL 3)

    uint64_t tss_base = (uint64_t)&tss;
    gdt[5] = (sizeof(tss_t) - 1) | ((tss_base & 0xFFFFFF) << 16) |
             (0x89ULL << 40) | ((tss_base & 0xFF000000) << 32);
    gdt[6] = tss_base >> 32;

    gdtr.limit = sizeof(gdt) - 1;
    gdtr.base = (uint64_t)&gdt;

    __asm__ __volatile__(
        "lgdt %0\n\t"
        "push $0x08\n\t"
        "lea 1f(%%rip), %%rax\n\t"
        "push %%rax\n\t"
        "lretq\n\t"
        "1:\n\t"
        "mov $0x10, %%ax\n\t"
        "mov %%ax, %%ds\n\t"
        "mov %%ax, %%es\n\t"
        "mov %%ax, %%fs\n\t"
        "mov %%ax, %%gs\n\t"
        "mov %%ax, %%ss\n\t"
        "mov $0x28, %%ax\n\t"
        "ltr %%ax"
        : : "m"(gdtr) : "rax", "memory"
    );
}

void set_tss_rsp0(uint64_t rsp0) {
    tss.rsp0 = rsp0;
}
