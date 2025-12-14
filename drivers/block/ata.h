#ifndef ATA_H
#define ATA_H

#include <types.h>

// I/O Ports for Primary Bus
#define ATA_DATA        0x1F0   // Read/Write Data
#define ATA_ERROR       0x1F1   // Read: Error / Write: Features
#define ATA_SECTOR_CNT  0x1F2   // Sector Count
#define ATA_LBA_LOW     0x1F3   // LBA bits 0-7
#define ATA_LBA_MID     0x1F4   // LBA bits 8-15
#define ATA_LBA_HIGH    0x1F5   // LBA bits 16-23
#define ATA_DRIVE_HEAD  0x1F6   // Drive Select & LBA 24-27
#define ATA_STATUS      0x1F7   // Read: Status / Write: Command

// Status Flags (From ATA_STATUS port)
#define ATA_SR_BSY      0x80    // Busy
#define ATA_SR_DRDY     0x40    // Drive Ready
#define ATA_SR_DRQ      0x08    // Data Request Ready (Data is ready to transfer)
#define ATA_SR_ERR      0x01    // Error

// Commands
#define ATA_CMD_READ_PIO    0x20
#define ATA_CMD_WRITE_PIO   0x30

// Functions
void ata_read_sectors(uint32_t lba, uint8_t count, uint16_t *buffer);
void ata_write_sectors(uint32_t lba, uint8_t count, uint16_t *buffer);

#endif
