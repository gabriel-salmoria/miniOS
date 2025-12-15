#ifndef MBR_H
#define MBR_H

#include <types.h>

// Force 1-byte alignment
typedef struct {
    uint8_t  boot_indicator;
    uint8_t  start_head;
    uint8_t  start_sector;
    uint8_t  start_cyl;
    uint8_t  sys_id;         // We look for 0x83 here
    uint8_t  end_head;
    uint8_t  end_sector;
    uint8_t  end_cyl;
    uint32_t start_lba;
    uint32_t total_sectors;
} __attribute__((packed)) mbr_partition_entry_t;

typedef struct {
    uint8_t               bootstrap[446];
    mbr_partition_entry_t partitions[4];
    uint16_t              signature;      // 0xAA55
} __attribute__((packed)) mbr_t;

uint32_t get_partition_offset();

#endif
