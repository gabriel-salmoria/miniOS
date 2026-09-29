#include "ata.h"
#include "drivers/apic/ports.h"

void ata_wait_bsy() {
    while (inb(ATA_STATUS) & ATA_SR_BSY);
}

void ata_wait_drq() {
    while (!(inb(ATA_STATUS) & ATA_SR_DRQ));
}

void ata_read_sectors(uint32_t lba, uint8_t count, uint16_t *buffer, int slave) {
    uint8_t drive_mode = slave ? 0xF0 : 0xE0;

    // 1. Select Drive
    outb(ATA_DRIVE_HEAD, drive_mode | ((lba >> 24) & 0x0F));


    // 2. [NEW] 400ns Delay (Read Status 4 times)
    // This allows the drive circuitry to settle before we send more commands.
    for (int k = 0; k < 4; k++) inb(ATA_STATUS);

    // 3. Continue setup
    outb(ATA_ERROR, 0x00);
    outb(ATA_SECTOR_CNT, count);
    outb(ATA_LBA_LOW,  (uint8_t) lba);
    outb(ATA_LBA_MID,  (uint8_t)(lba >> 8));
    outb(ATA_LBA_HIGH, (uint8_t)(lba >> 16));

    outb(ATA_STATUS, ATA_CMD_READ_PIO);



    for (int i = 0; i < count; i++) {
        ata_wait_bsy();
        ata_wait_drq();
        insw(ATA_DATA, &buffer[i * 256], 256);
    }


}

void ata_write_sectors(uint32_t lba, uint8_t count, uint16_t *buffer, int slave) {
    uint8_t drive_mode = slave ? 0xF0 : 0xE0;

    for (int k = 0; k < 4; k++) inb(ATA_STATUS);

    ata_wait_bsy();
    outb(ATA_DRIVE_HEAD, drive_mode | ((lba >> 24) & 0x0F));
    outb(ATA_ERROR, 0x00);
    outb(ATA_SECTOR_CNT, count);
    outb(ATA_LBA_LOW,  (uint8_t) lba);
    outb(ATA_LBA_MID,  (uint8_t)(lba >> 8));
    outb(ATA_LBA_HIGH, (uint8_t)(lba >> 16));

    outb(ATA_STATUS, ATA_CMD_WRITE_PIO);

    for (int i = 0; i < count; i++) {
        ata_wait_bsy();
        ata_wait_drq();
        outsw(ATA_DATA, &buffer[i * 256], 256);
    }
}
