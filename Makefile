BOOT_CC = x86_64-w64-mingw32-gcc
BOOT_CFLAGS = -Wall -Wextra -m64 -ffreestanding -fshort-wchar -fPIC -fPIE -mno-red-zone -fno-stack-protector -Iinclude
BOOT_LDFLAGS = -nostdlib -Wl,-T,linker.ld -Wl,--image-base,0x400000 -Wl,-mi386pep -Wl,--subsystem,10 -Wl,-e,efi_main

QEMU = qemu-system-x86_64
BUILD_DIR = build
BOOT_DIR = bootloader

.PHONY: all run clean

all: $(BUILD_DIR)/os-image.img

run: all
	@echo "  [RUN]   $(BUILD_DIR)/os-image.img"
	@$(QEMU) -bios /usr/share/edk2/x64/OVMF.4m.fd -drive format=raw,file=$(BUILD_DIR)/os-image.img

$(BUILD_DIR)/main.o: $(BOOT_DIR)/main.c
	@mkdir -p $(BUILD_DIR)
	@echo "  [CC]    $<"
	@$(BOOT_CC) $(BOOT_CFLAGS) -c $< -o $@

$(BUILD_DIR)/BOOTX64.EFI: $(BUILD_DIR)/main.o
	@echo "  [LD]    $@"
	@$(BOOT_CC) $(BOOT_LDFLAGS) $< -o $@

$(BUILD_DIR)/os-image.img: $(BUILD_DIR)/BOOTX64.EFI
	@echo "  [IMG]   $@"
	@dd if=/dev/zero of=$@ bs=1M count=64 status=none
	@mformat -i $@ -F ::
	@mmd -i $@ ::/EFI
	@mmd -i $@ ::/EFI/BOOT
	@mcopy -i $@ $< ::/EFI/BOOT/BOOTX64.EFI

clean:
	@echo "  [CLEAN] $(BUILD_DIR)"
	@rm -rf $(BUILD_DIR)/*
