#include "ata.h"
#include "drivers/ports.h" // for inb, outb, insw, outsw

// Wait for the drive to be ready (not busy)
void ata_wait_bsy() {
    while (inb(ATA_STATUS) & ATA_SR_BSY);
}

// Wait until the drive is ready to transfer data
void ata_wait_drq() {
    while (!(inb(ATA_STATUS) & ATA_SR_DRQ));
}

void ata_read_sectors(uint32_t lba, uint8_t count, uint16_t *buffer) {
    // 1. Select Master Drive + Top 4 bits of LBA
    // 0xE0 = 11100000 (Mode LBA, Master Drive)
    outb(ATA_DRIVE_HEAD, 0xE0 | ((lba >> 24) & 0x0F));

    // 2. Send NULL to Error Port (just in case)
    outb(ATA_ERROR, 0x00);

    // 3. Send Sector Count
    outb(ATA_SECTOR_CNT, count);

    // 4. Send LBA Address (Low -> Mid -> High)
    outb(ATA_LBA_LOW,  (uint8_t) lba);
    outb(ATA_LBA_MID,  (uint8_t)(lba >> 8));
    outb(ATA_LBA_HIGH, (uint8_t)(lba >> 16));

    // 5. Send Command
    outb(ATA_STATUS, ATA_CMD_READ_PIO);

    // 6. Read Data Loop
    for (int i = 0; i < count; i++) {
        ata_wait_bsy(); // Wait for controller
        ata_wait_drq(); // Wait for data ready

        insw(ATA_DATA, &buffer[i * 256], 256);
    }
}

// Minimal Write (Optional for now)
void ata_write_sectors(uint32_t lba, uint8_t count, uint16_t *buffer) {
    ata_wait_bsy();
    outb(ATA_DRIVE_HEAD, 0xE0 | ((lba >> 24) & 0x0F));
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
