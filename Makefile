# --- Configuration ---
CC = gcc
LD = ld
ASM = nasm
QEMU = qemu-system-x86_64

# Addresses
KERNEL_ADDR = 0x1000

# Directories
BUILD_DIR = build
ARCH_DIR = arch/i386/boot
KERNEL_DIR = kernel
DRIVERS_DIR = drivers
INCLUDE_DIR = include

# Compiler Flags
# -m32: 32-bit output
# -I: Include directories for C headers
# -ffreestanding -nostdlib: Minimal environment, no host runtime
# -fno-pic: Disable Position Independent Code (absolute addressing)
# -c: Compile only
CFLAGS = -m32 -I$(INCLUDE_DIR) -I$(ARCH_DIR) -I$(DRIVERS_DIR) -ffreestanding -nostdlib -fno-builtin -fno-stack-protector -fno-pic -c

# Linker Flags
# -m elf_i386: Emulate 32-bit linker
# -Ttext 0x1000: Set entry address
# --oformat binary: Output raw code
LDFLAGS = -m elf_i386 -Ttext $(KERNEL_ADDR) --oformat binary

# Assembler Flags
# -I: Include path so boot.asm can find gdt.asm/print.asm
ASMFLAGS = -f bin -I $(ARCH_DIR)/

# --- Source and Object Files ---
# Find all C source files recursively
C_SOURCES = $(shell find . -name "*.c")
# Map all .c files to object files in the build directory
OBJ = $(patsubst %.c, $(BUILD_DIR)/%.o, $(C_SOURCES))

# --- Targets ---

.PHONY: all run clean

all: $(BUILD_DIR)/os-image.bin

run: all
	$(QEMU) -drive format=raw,file=$(BUILD_DIR)/os-image.bin

# Concatenate Bootloader + Kernel
$(BUILD_DIR)/os-image.bin: $(BUILD_DIR)/boot.bin $(BUILD_DIR)/kernel.bin
	cat $^ > $@

# Link the C objects into a single kernel binary
$(BUILD_DIR)/kernel.bin: $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $^

# Rule to compile any C file. $^ is the .c source, $@ is the .o object
# Creates intermediate directories if needed (for when drivers/ is populated)
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

# Assemble Bootloader
$(BUILD_DIR)/boot.bin: $(ARCH_DIR)/boot.asm $(ARCH_DIR)/gdt.asm $(ARCH_DIR)/print.asm
	$(ASM) $(ASMFLAGS) $(ARCH_DIR)/boot.asm -o $@

clean:
	rm -rf $(BUILD_DIR)/*
