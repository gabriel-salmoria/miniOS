# --- Configuration ---
CC = gcc
LD = ld
ASM = nasm
QEMU = qemu-system-x86_64

BUILD_DIR = build

# Compiler Flags
CFLAGS = -m32 -ffreestanding -nostdlib -fno-builtin -fno-stack-protector -fno-pic -c
CFLAGS += -I. -Iinclude

LDFLAGS = -m elf_i386 -T linker.ld

# --- Recursive Source Discovery ---
C_SOURCES = $(patsubst ./%, %, $(shell find . -name "*.c"))
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
	@echo "  [RUN]   $(BUILD_DIR)/os-image.bin"
	@$(QEMU) -drive format=raw,file=$(BUILD_DIR)/os-image.bin,index=0,if=ide

# Disk Image (Partitioned: MBR + Kernel + Ext2)
$(BUILD_DIR)/os-image.bin: $(BUILD_DIR)/boot.bin $(BUILD_DIR)/kernel.bin README.md
	@echo "  [IMG]   $@"
	@dd if=/dev/zero of=$@ bs=1M count=10 2>/dev/null
	@dd if=$(BUILD_DIR)/boot.bin of=$@ conv=notrunc 2>/dev/null
	@echo "start=2048, type=83, bootable" | sfdisk $@ >/dev/null 2>&1
	@dd if=$(BUILD_DIR)/kernel.bin of=$@ seek=1 conv=notrunc 2>/dev/null
	@dd if=/dev/zero of=$(BUILD_DIR)/fs.img bs=1M count=9 2>/dev/null
	@/sbin/mkfs.ext2 -q $(BUILD_DIR)/fs.img
	@debugfs -w -R "write README.md README.md" $(BUILD_DIR)/fs.img >/dev/null 2>&1
	@dd if=$(BUILD_DIR)/fs.img of=$@ seek=2048 conv=notrunc 2>/dev/null
	@rm $(BUILD_DIR)/fs.img

# Kernel Binary
$(BUILD_DIR)/kernel.bin: $(ALL_OBJS) linker.ld
	@echo "  [LD]    $@"
	@$(LD) $(LDFLAGS) -o $@ $(ALL_OBJS)

# Generic C Rule
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "  [CC]    $<"
	@$(CC) $(CFLAGS) $< -o $@

# Generic Assembly Rule
$(BUILD_DIR)/%.o: %.asm
	@mkdir -p $(dir $@)
	@echo "  [ASM]   $<"
	@$(ASM) -f elf32 $< -o $@

# Bootloader
$(BUILD_DIR)/boot.bin: arch/i386/boot/boot.asm
	@mkdir -p $(dir $@)
	@echo "  [NASM]  $<"
	@$(ASM) -f bin -I arch/i386/boot/ $< -o $@

clean:
	@echo "  [CLEAN] $(BUILD_DIR)"
	@rm -rf $(BUILD_DIR)/*
