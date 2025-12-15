#include "kernel/fs/mbr.h"
#include "drivers/block/ata.h"
#include "drivers/screen.h"
#include "kernel/mem/heap.h"
#include "libc/string.h" // For hex_to_ascii

uint32_t get_partition_offset() {
    mbr_t *mbr = (mbr_t*)kmalloc(512);

    // Read Master Drive (Slave=0)
    ata_read_sectors(0, 1, (uint16_t*)mbr, 0);

    // 1. Verify Signature
    if (mbr->signature != 0xAA55) {
        kprint("MBR Bad Signature: 0x");
        char sig_buf[16];
        hex_to_ascii(mbr->signature, sig_buf);
        kprint(sig_buf);
        kprint("\n");
        kfree(mbr);
        return 0;
    }

    // 2. Scan Partitions
    for (int i = 0; i < 4; i++) {
        if (mbr->partitions[i].sys_id == 0x83) { // 0x83 = Linux Native
            kprint("[MBR] - Found Linux Partition.\n");
            uint32_t offset = mbr->partitions[i].start_lba;
            kfree(mbr);
            return offset;
        }
    }

    kprint("No Linux Partition Found.\n");
    kfree(mbr);
    return 0;
}
