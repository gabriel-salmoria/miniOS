#include "kernel/fs/ext2.h"
#include "drivers/block/ata.h"
#include "drivers/screen.h"
#include "kernel/mem/heap.h"
#include "libc/string.h"

// --- Globals ---
// CHANGE 1: 'sb' is now a pointer, not a struct
static uint32_t sb_raw_data[256];

static ext2_superblock_t *sb = 0;
static uint32_t block_size;
static uint32_t sectors_per_block;
static uint32_t inodes_per_group;
static uint32_t bg_desc_table_offset; // This is a Block Group Index, not byte offset

// --- Internal Helpers ---

static uint32_t block_to_lba(uint32_t block) {
    return block * sectors_per_block;
}

static void ext2_read_block(uint32_t block_num, uint8_t *buffer) {
    if (block_num == 0) return;
    uint32_t lba = block_to_lba(block_num);
    ata_read_sectors(lba, sectors_per_block, (uint16_t*)buffer, 1);
}


static void get_group_descriptor(uint32_t group_index, ext2_group_desc_t *desc) {
    if (!desc) {
        kprint("PANIC: desc is NULL\n");
        return;
    }

    uint32_t descriptor_size = sizeof(ext2_group_desc_t);
    uint32_t descriptors_per_block = block_size / descriptor_size;

    if (descriptor_size == 0 || descriptors_per_block == 0) {
        kprint("PANIC: invalid descriptor sizing\n");
        return;
    }

    uint32_t block_offset = group_index / descriptors_per_block;
    uint32_t entry_offset = group_index % descriptors_per_block;

    uint8_t *temp_buf = kmalloc(block_size);
    if (!temp_buf) {
        kprint("PANIC: kmalloc failed\n");
        return;
    }

    ext2_read_block(bg_desc_table_offset + block_offset, temp_buf);

    uint32_t off = entry_offset * descriptor_size;
    if (off + sizeof(ext2_group_desc_t) > block_size) {
        kprint("PANIC: group descriptor out of bounds\n");
        kfree(temp_buf);
        return;
    }

    ext2_group_desc_t *entry = (ext2_group_desc_t *)(temp_buf + off);
    memcpy(desc, entry, sizeof(ext2_group_desc_t));

    kfree(temp_buf);
}

// --- Main Functionality ---

void ext2_read_inode(uint32_t inode_num, ext2_inode_t *inode_out) {
    if (inode_num == 0) return;

    // CHANGE 2: Use 'sb->' instead of 'sb.'
    uint32_t group_index = (inode_num - 1) / inodes_per_group;
    uint32_t table_index = (inode_num - 1) % inodes_per_group;



    ext2_group_desc_t group_desc;
    get_group_descriptor(group_index, &group_desc);


    // CHANGE 3: Use 'sb->'
    uint32_t inode_size = sb->inode_size;
    uint32_t inodes_per_block = block_size / inode_size;

    uint32_t block_offset = table_index / inodes_per_block;
    uint32_t index_in_block = table_index % inodes_per_block;

    uint32_t target_block = group_desc.inode_table + block_offset;

    uint8_t *buf = (uint8_t*)kmalloc(block_size);
    ext2_read_block(target_block, buf);


    ext2_inode_t *ptr = (ext2_inode_t*)(buf + (index_in_block * inode_size));

    memcpy(inode_out, ptr, sizeof(ext2_inode_t));

    kfree(buf);
}

void ext2_read_file(ext2_inode_t *inode, uint8_t *buf) {
    int blocks_needed = (inode->size + block_size - 1) / block_size;

    if (blocks_needed > 12) {
        kprint("WARNING: Large files (indirect blocks) not yet supported!\n");
        blocks_needed = 12;
    }

    for (int i = 0; i < blocks_needed; i++) {
        ext2_read_block(inode->block[i], buf + (i * block_size));
    }
}

uint32_t ext2_find_file(ext2_inode_t *dir_inode, const char *name) {

    if ((dir_inode->mode & EXT2_S_IFDIR) == 0) return 0;


    uint8_t *buf = (uint8_t*)kmalloc(block_size);

    for (int i = 0; i < 12; i++) {
        uint32_t block_id = dir_inode->block[i];
        if (block_id == 0) break;

        ext2_read_block(block_id, buf);

        uint32_t offset = 0;
        while (offset < block_size) {
            ext2_dir_entry_t *entry = (ext2_dir_entry_t*)(buf + offset);

            if (entry->inode != 0) {
                int name_len = strlen(name);
                if (entry->name_len == name_len && strcmp(name, entry->name) == 0) {
                    uint32_t result_inode = entry->inode;
                    kfree(buf);
                    return result_inode;
                }
            }
            offset += entry->rec_len;
        }
    }
    kfree(buf);
    return 0;
}

// --- Initialization ---

void ext2_init() {
    // 1. Allocate Superblock on Heap (Safe Memory)
    sb = (ext2_superblock_t*)sb_raw_data;

    if (sb == 0) {
        kprint("PANIC: Ext2 Kmalloc Failed!\n");
        return;
    }


    // 2. Read Superblock into the allocated memory
    // Note: 'sb' is already a pointer, so we cast it directly.
    ata_read_sectors(2, 2, (uint16_t*)sb, 1);

    // CHANGE 4: Use 'sb->' for member access
    if (sb->magic != EXT2_SIGNATURE) {
        kprint("Ext2: Invalid Magic Signature!\n");
        return;
    }


    // 3. Parse geometry
    block_size = 1024 << sb->log_block_size;
    sectors_per_block = block_size / 512;
    inodes_per_group = sb->inodes_per_group;

    bg_desc_table_offset = (block_size == 1024) ? 2 : 1;

    kprint("Ext2: Filesystem Mounted.\n");



    // --- Test Code ---
    ext2_inode_t root_inode;


    ext2_read_inode(2, &root_inode);



    uint32_t file_inode_num = ext2_find_file(&root_inode, "README.md");


    if (file_inode_num > 0) {
        kprint("Found README.md!\n");

        ext2_inode_t file_inode;
        ext2_read_inode(file_inode_num, &file_inode);

        char *file_buf = (char*)kmalloc(file_inode.size + 1);
        ext2_read_file(&file_inode, (uint8_t*)file_buf);
        file_buf[file_inode.size] = '\0';

        kprint("File Contents:\n");
        kprint(file_buf);
        kprint("\n");
        kfree(file_buf);
    } else {
        kprint("README.md not found in root.\n");
    }
}
