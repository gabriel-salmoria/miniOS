#include "kernel/fs/vfs.h"
#include "drivers/keyboard.h"
#include "drivers/screen.h"

vfs_node_t *vfs_stdin;
vfs_node_t *vfs_stdout;

static vfs_node_t node_stdin;
static vfs_node_t node_stdout;

static uint32_t stdin_read(vfs_node_t *node, uint64_t offset, uint32_t size, uint8_t *buffer) {
    (void)node; (void)offset;
    for (uint32_t i = 0; i < size; i++) {
        buffer[i] = kbd_getchar();
    }
    return size;
}

static uint32_t stdout_write(vfs_node_t *node, uint64_t offset, uint32_t size, uint8_t *buffer) {
    (void)node; (void)offset;
    for (uint32_t i = 0; i < size; i++) {
        char str[2] = {buffer[i], 0};
        kprint(str);
    }
    return size;
}

void vfs_init() {
    node_stdin.read = stdin_read;
    node_stdin.write = 0;
    vfs_stdin = &node_stdin;

    node_stdout.read = 0;
    node_stdout.write = stdout_write;
    vfs_stdout = &node_stdout;
}
