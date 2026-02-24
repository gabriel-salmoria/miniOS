#ifndef USER_SYSCALL_H
#define USER_SYSCALL_H

#include <types.h>

int write(int fd, const void *buf, uint32_t count);
int read(int fd, void *buf, uint32_t count);

void exit(int status);
void yield();

int open(const char *path);
int close(int fd);

#endif
