# --- Configuration ---
CC = gcc
LD = ld
ASM = nasm
QEMU = qemu-system-x86_64

# Directories
BUILD_DIR = build
ARCH_DIR = arch/i386/boot
KERNEL_DIR = kernel
DRIVERS_DIR = drivers
INCLUDE_DIR = include

# Compiler Flags
CFLAGS = -m32 -I$(INCLUDE_DIR) -I$(ARCH_DIR) -I$(DRIVERS_DIR) -ffreestanding -nostdlib -fno-builtin -fno-stack-protector -fno-pic -c

# Linker Flags (CRITICAL FIX: Keep -m elf_i386)
# -m elf_i386: Force 32-bit emulation
# -T linker.ld: Use our custom script for memory layout
LDFLAGS = -m elf_i386 -T linker.ld

# Assembler Flags
ASMFLAGS = -f bin -I $(ARCH_DIR)/

# --- Source and Object Files ---

# 1. Find all C files
RAW_SOURCES = $(shell find . -name "*.c")

# 2. Clean paths (remove ./ prefix)
C_SOURCES = $(patsubst ./%, %, $(RAW_SOURCES))

# 3. Create list of object files
ALL_OBJ = $(patsubst %.c, $(BUILD_DIR)/%.o, $(C_SOURCES))

# 4. Force kernel/hello.o to be first
KERNEL_OBJ = $(BUILD_DIR)/kernel/hello.o
OBJ = $(KERNEL_OBJ) $(filter-out $(KERNEL_OBJ), $(ALL_OBJ))

# --- Targets ---

.PHONY: all run clean

all: $(BUILD_DIR)/os-image.bin

run: all
	$(QEMU) -drive format=raw,file=$(BUILD_DIR)/os-image.bin

# Disk Image
$(BUILD_DIR)/os-image.bin: $(BUILD_DIR)/boot.bin $(BUILD_DIR)/kernel.bin
	dd if=/dev/zero of=$@ bs=512 count=20
	dd if=$(BUILD_DIR)/boot.bin of=$@ conv=notrunc
	dd if=$(BUILD_DIR)/kernel.bin of=$@ seek=1 conv=notrunc

# Linker
$(BUILD_DIR)/kernel.bin: $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $^

# Compile C files
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

# Assemble Bootloader
$(BUILD_DIR)/boot.bin: $(ARCH_DIR)/boot.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)/*
