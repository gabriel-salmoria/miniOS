# --- Configuration ---
CC = gcc
LD = ld
ASM = nasm
QEMU = qemu-system-x86_64

BUILD_DIR = build

# Compiler Flags
# -I. allows including via "kernel/cpu/isr.h" (Root-relative)
# -Iinclude allows including <types.h> (Global)
CFLAGS = -m32 -ffreestanding -nostdlib -fno-builtin -fno-stack-protector -fno-pic -c
CFLAGS += -I. -Iinclude

LDFLAGS = -m elf_i386 -T linker.ld

# --- Recursive Source Discovery ---

# 1. Find C files and strip the leading "./"
# Result: kernel/main.c instead of ./kernel/main.c
C_SOURCES = $(patsubst ./%, %, $(shell find . -name "*.c"))

# 2. Find ASM files (excluding boot), strip "./"
ASM_SOURCES = $(patsubst ./%, %, $(shell find . -name "*.asm" ! -path "*/boot/*"))

# Create list of Object files to build
OBJ = $(patsubst %.c, $(BUILD_DIR)/%.o, $(C_SOURCES)) \
      $(patsubst %.asm, $(BUILD_DIR)/%.o, $(ASM_SOURCES))

# Ensure main.o is linked FIRST
MAIN_OBJ = $(BUILD_DIR)/kernel/main.o
ALL_OBJS = $(MAIN_OBJ) $(filter-out $(MAIN_OBJ), $(OBJ))

# --- Targets ---

.PHONY: all run clean

all: $(BUILD_DIR)/os-image.bin

run: all
	$(QEMU) -drive format=raw,file=$(BUILD_DIR)/os-image.bin,index=0,if=ide

# Disk Image (Partitioned: MBR + Kernel + Ext2)
# Makefile

$(BUILD_DIR)/os-image.bin: $(BUILD_DIR)/boot.bin $(BUILD_DIR)/kernel.bin README.md
	# 1. Create a blank 10MB disk image
	dd if=/dev/zero of=$@ bs=1M count=10

	# 2. Write Bootloader (Sector 0) -- DO THIS FIRST
	dd if=$(BUILD_DIR)/boot.bin of=$@ conv=notrunc

	# 3. Create MBR Partition Table -- DO THIS SECOND
	# sfdisk will preserve the code in Sector 0 we just wrote
	echo "start=2048, type=83, bootable" | sfdisk $@

	# 4. Write Kernel (Sector 1)
	dd if=$(BUILD_DIR)/kernel.bin of=$@ seek=1 conv=notrunc

	# 5. Create and Inject Filesystem
	dd if=/dev/zero of=$(BUILD_DIR)/fs.img bs=1M count=9
	/sbin/mkfs.ext2 $(BUILD_DIR)/fs.img
	debugfs -w -R "write README.md README.md" $(BUILD_DIR)/fs.img
	dd if=$(BUILD_DIR)/fs.img of=$@ seek=2048 conv=notrunc
	rm $(BUILD_DIR)/fs.img


# Kernel Binary
$(BUILD_DIR)/kernel.bin: $(ALL_OBJS) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(ALL_OBJS)

# Generic C Rule (Mirrors folder structure to build/)
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

# Generic Assembly Rule
$(BUILD_DIR)/%.o: %.asm
	@mkdir -p $(dir $@)
	$(ASM) -f elf32 $< -o $@

# Bootloader
$(BUILD_DIR)/boot.bin: arch/i386/boot/boot.asm
	@mkdir -p $(dir $@)
	$(ASM) -f bin -I arch/i386/boot/ $< -o $@

clean:
	rm -rf $(BUILD_DIR)/*
