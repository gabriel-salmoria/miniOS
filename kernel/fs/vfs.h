#ifndef VFS_H
#define VFS_H

#include <types.h>

typedef struct vfs_node {
    uint32_t (*read)(struct vfs_node *node, uint64_t offset, uint32_t size, uint8_t *buffer);
    uint32_t (*write)(struct vfs_node *node, uint64_t offset, uint32_t size, uint8_t *buffer);
} vfs_node_t;

extern vfs_node_t *vfs_stdin;
extern vfs_node_t *vfs_stdout;

void vfs_init();

#endif
