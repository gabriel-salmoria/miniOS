#include "efi.h"

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    (void)ImageHandle; // Unused for now

    // Clear the screen and print a test string
    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, u"ShitOS 64-bit UEFI Bootloader\r\n");

    // Halt execution
    while (1) {
        __asm__ __volatile__("hlt");
    }

    return EFI_SUCCESS;
}
