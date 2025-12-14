# --- Configuration ---
CC = gcc
LD = ld
ASM = nasm
QEMU = qemu-system-x86_64

# Project Structure
BUILD_DIR = build
ARCH_DIR = arch/i386
KERNEL_DIR = kernel
DRIVERS_DIR = drivers
INCLUDE_DIR = include
USER_DIR = user
LIBC_DIR = libc

# Compiler Flags
# REFLECTED STRUCTURE: Added -I$(KERNEL_DIR)
# Now you can #include "filename.h" if it exists in include/, drivers/, or kernel/
CFLAGS = -m32  -ffreestanding -nostdlib -fno-builtin -fno-stack-protector -fno-pic -c
CFLAGS += -I$(INCLUDE_DIR) -I$(KERNEL_DIR) -I$(ARCH_DIR)/boot -I$(DRIVERS_DIR) -I$(USER_DIR) -I$(LIBC_DIR)

# Linker Flags
LDFLAGS = -m elf_i386 -T linker.ld

# --- Sources ---

# 1. C Sources (Automatically finds new files like drivers/pic.c)
C_SOURCES = $(patsubst ./%, %, $(shell find . -name "*.c"))
C_OBJ = $(patsubst %.c, $(BUILD_DIR)/%.o, $(C_SOURCES))

# 2. Kernel Assembly
ASM_SOURCE = $(ARCH_DIR)/interrupt.asm
ASM_OBJ = $(BUILD_DIR)/$(ARCH_DIR)/interrupt.o

# 3. Object Management
# We force main.o to be the first object linked so it's at 0x1000
KERNEL_ENTRY = $(BUILD_DIR)/$(KERNEL_DIR)/main.o
OBJ = $(KERNEL_ENTRY) $(ASM_OBJ) $(filter-out $(KERNEL_ENTRY), $(C_OBJ))

# --- Targets ---

.PHONY: all run clean

all: $(BUILD_DIR)/os-image.bin

run: all
	$(QEMU) -drive format=raw,file=$(BUILD_DIR)/os-image.bin

# Disk Image
$(BUILD_DIR)/os-image.bin: $(BUILD_DIR)/boot.bin $(BUILD_DIR)/kernel.bin
	dd if=/dev/zero of=$@ bs=512 count=200
	dd if=$(BUILD_DIR)/boot.bin of=$@ conv=notrunc
	dd if=$(BUILD_DIR)/kernel.bin of=$@ seek=1 conv=notrunc

# Kernel Binary
$(BUILD_DIR)/kernel.bin: $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $^

# Generic C Rule (Mirrors directory structure in build/)
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

# Kernel Assembly Rule
$(ASM_OBJ): $(ASM_SOURCE)
	@mkdir -p $(dir $@)
	$(ASM) -f elf32 $< -o $@

# Bootloader Rule
$(BUILD_DIR)/boot.bin: $(ARCH_DIR)/boot/boot.asm
	@mkdir -p $(dir $@)
	$(ASM) -f bin -I $(ARCH_DIR)/boot/ $< -o $@

clean:
	rm -rf $(BUILD_DIR)/*
