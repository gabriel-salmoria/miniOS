#ifndef EXT2_H
#define EXT2_H

#include <types.h>

#define EXT2_SIGNATURE      0xEF53
#define EXT2_ROOT_INO       2

// File Types (for Directory Entries)
#define EXT2_FT_UNKNOWN     0
#define EXT2_FT_REG_FILE    1
#define EXT2_FT_DIR         2
#define EXT2_FT_CHRDEV      3
#define EXT2_FT_BLKDEV      4
#define EXT2_FT_FIFO        5
#define EXT2_FT_SOCK        6
#define EXT2_FT_SYMLINK     7

// Inode Modes (st_mode)
#define EXT2_S_IFMT         0xF000  // Format Mask
#define EXT2_S_IFSOCK       0xC000  // Socket
#define EXT2_S_IFLNK        0xA000  // Symbolic Link
#define EXT2_S_IFREG        0x8000  // Regular File
#define EXT2_S_IFBLK        0x6000  // Block Device
#define EXT2_S_IFDIR        0x4000  // Directory
#define EXT2_S_IFCHR        0x2000  // Character Device
#define EXT2_S_IFIFO        0x1000  // FIFO

// --- 1. Superblock (1024 bytes) ---
typedef struct {
    uint32_t inodes_count;
    uint32_t blocks_count;
    uint32_t r_blocks_count;
    uint32_t free_blocks_count;
    uint32_t free_inodes_count;
    uint32_t first_data_block;
    uint32_t log_block_size;    // 1024 << log_block_size
    uint32_t log_frag_size;
    uint32_t blocks_per_group;
    uint32_t frags_per_group;
    uint32_t inodes_per_group;
    uint32_t mtime;
    uint32_t wtime;
    uint16_t mnt_count;
    uint16_t max_mnt_count;
    uint16_t magic;             // 0xEF53
    uint16_t state;
    uint16_t errors;
    uint16_t minor_rev_level;
    uint32_t lastcheck;
    uint32_t checkinterval;
    uint32_t creator_os;
    uint32_t rev_level;
    uint16_t def_resuid;
    uint16_t def_resgid;

    // -- EXT2_DYNAMIC_REV Specific --
    uint32_t first_ino;         // First non-reserved inode
    uint16_t inode_size;        // Size of inode structure (usually 128)
    uint16_t block_group_nr;    // Block group # of this superblock
    uint32_t feature_compat;
    uint32_t feature_incompat;
    uint32_t feature_ro_compat;
    uint8_t  uuid[16];
    char     volume_name[16];
    char     last_mounted[64];
    uint32_t algo_bitmap;

    // -- Performance Hints --
    uint8_t  prealloc_blocks;
    uint8_t  prealloc_dir_blocks;
    uint16_t padding1;

    // -- Journaling Support --
    uint8_t  journal_uuid[16];
    uint32_t journal_inum;
    uint32_t journal_dev;
    uint32_t last_orphan;

    // -- Directory Indexing Support --
    uint32_t hash_seed[4];
    uint8_t  def_hash_version;
    uint8_t  jnl_backup_type;
    uint16_t desc_size;
    uint32_t default_mount_opts;
    uint32_t first_meta_bg;
    uint8_t  mkfs_time[16];
    uint32_t jnl_blocks[17];    // Backup of journal inode

    // Pad to 1024 bytes
    uint8_t  reserved[190];
} __attribute__((packed)) ext2_superblock_t;

// --- 2. Block Group Descriptor (32 bytes) ---
typedef struct {
    uint32_t block_bitmap;      // Block containing block usage bitmap
    uint32_t inode_bitmap;      // Block containing inode usage bitmap
    uint32_t inode_table;       // Block containing inode table
    uint16_t free_blocks_count;
    uint16_t free_inodes_count;
    uint16_t used_dirs_count;
    uint16_t pad;
    uint8_t  reserved[12];
} __attribute__((packed)) ext2_group_desc_t;

// --- 3. Inode (128 bytes) ---
typedef struct {
    uint16_t mode;              // Permissions & Type
    uint16_t uid;
    uint32_t size;              // Size in bytes
    uint32_t atime;             // Access time
    uint32_t ctime;             // Creation time
    uint32_t mtime;             // Modification time
    uint32_t dtime;             // Deletion time
    uint16_t gid;
    uint16_t links_count;
    uint32_t blocks;            // 512-byte blocks reserved for this file
    uint32_t flags;
    uint32_t osd1;              // OS Dependent

    // Pointers to data blocks
    uint32_t block[15];         // 0-11: Direct, 12: Singly, 13: Doubly, 14: Triply

    uint32_t generation;        // File version (for NFS)
    uint32_t file_acl;
    uint32_t dir_acl;
    uint32_t faddr;             // Fragment address
    uint8_t  osd2[12];          // OS Dependent
} __attribute__((packed)) ext2_inode_t;

// --- 4. Directory Entry (Variable size) ---
typedef struct {
    uint32_t inode;             // Inode number
    uint16_t rec_len;           // Total size of this entry (pointer to next)
    uint8_t  name_len;          // Name length
    uint8_t  file_type;         // File type (see defines above)
    char     name[];            // Filename (Not null-terminated)
} __attribute__((packed)) ext2_dir_entry_t;

// Functions
void ext2_init();

#endif
