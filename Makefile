BOOT_CC = x86_64-w64-mingw32-gcc
BOOT_CFLAGS = -Wall -Wextra -mno-red-zone -m64 -ffreestanding -fshort-wchar -fPIC -fPIE -fno-stack-protector -Iinclude
BOOT_LDFLAGS = -nostdlib -Wl,-T,bootloader/linker.ld -Wl,--image-base,0x400000 -Wl,-mi386pep -Wl,--subsystem,10 -Wl,-e,efi_main

KERNEL_CC = gcc
KERNEL_LD = ld
KERNEL_CFLAGS = -Wall -Wextra -m64 -ffreestanding -fno-stack-protector -fno-pic -mno-red-zone -mcmodel=large -mno-sse -mno-mmx -Iinclude -I.
KERNEL_LDFLAGS = -m elf_x86_64 -T linker.ld --oformat binary

QEMU = qemu-system-x86_64
BUILD_DIR = build

C_SOURCES = $(shell find kernel drivers libc user -name '*.c' ! -name 'main.c')
ASM_SOURCES = $(shell find arch/x86_64 -name '*.asm')

KERNEL_OBJS = $(BUILD_DIR)/kernel/main.o \
              $(patsubst %.c, $(BUILD_DIR)/%.o, $(C_SOURCES)) \
              $(patsubst %.asm, $(BUILD_DIR)/%.o, $(ASM_SOURCES))

.PHONY: all run clean

all: $(BUILD_DIR)/os-image.img

run: all
	@echo "  [RUN]   $(BUILD_DIR)/os-image.img"
	@$(QEMU) -bios /usr/share/edk2/x64/OVMF.4m.fd -drive format=raw,file=$(BUILD_DIR)/os-image.img

$(BUILD_DIR)/BOOTX64.EFI: bootloader/main.c bootloader/linker.ld
	@mkdir -p $(dir $@)
	@echo "  [BOOT]  $<"
	@$(BOOT_CC) $(BOOT_CFLAGS) -c $< -o $(BUILD_DIR)/main_boot.o
	@$(BOOT_CC) $(BOOT_LDFLAGS) $(BUILD_DIR)/main_boot.o -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "  [CC]    $<"
	@$(KERNEL_CC) $(KERNEL_CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.asm
	@mkdir -p $(dir $@)
	@echo "  [AS]    $<"
	@nasm -f elf64 $< -o $@

$(BUILD_DIR)/kernel.bin: $(KERNEL_OBJS) linker.ld
	@echo "  [LD]    $@"
	@$(KERNEL_LD) $(KERNEL_LDFLAGS) -o $@ $(KERNEL_OBJS)

$(BUILD_DIR)/os-image.img: $(BUILD_DIR)/BOOTX64.EFI $(BUILD_DIR)/kernel.bin font.psf
	@echo "  [IMG]   $@"
	@dd if=/dev/zero of=$@ bs=1M count=64 status=none
	@mformat -i $@ -F ::
	@mmd -i $@ ::/EFI ::/EFI/BOOT
	@mcopy -i $@ $(BUILD_DIR)/BOOTX64.EFI ::/EFI/BOOT/BOOTX64.EFI
	@mcopy -i $@ $(BUILD_DIR)/kernel.bin ::/kernel.bin
	@mcopy -i $@ font.psf ::/font.psf

clean:
	@echo "  [CLEAN] $(BUILD_DIR)"
	@rm -rf $(BUILD_DIR)/*
