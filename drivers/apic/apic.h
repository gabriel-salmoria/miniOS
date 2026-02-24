#ifndef APIC_H
#define APIC_H
#include <types.h>

void init_apic();
void apic_write(uint32_t reg, uint32_t val);
uint32_t apic_read(uint32_t reg);
void apic_eoi();

#endif
