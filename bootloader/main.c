#include "efi.h"
#include "shared_types.h"

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
    EFI_STATUS status;

    status = SystemTable->BootServices->LocateProtocol(&gopGuid, 0, (void **)&gop);
    if (status != EFI_SUCCESS) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, u"Failed to locate GOP\r\n");
        while (1) __asm__ __volatile__("hlt");
    }

    // Populate shared struct
    framebuffer_info_t fb_info;
    fb_info.base_address = (uint32_t *)gop->Mode->FrameBufferBase;
    fb_info.buffer_size  = gop->Mode->FrameBufferSize;
    fb_info.width        = gop->Mode->Info->HorizontalResolution;
    fb_info.height       = gop->Mode->Info->VerticalResolution;
    fb_info.pitch        = gop->Mode->Info->PixelsPerScanLine;

    SystemTable->ConOut->OutputString(SystemTable->ConOut, u"GOP Initialized. Ready to load kernel...\r\n");

    // TODO: 1. Use Simple File System Protocol to load kernel.elf
    // TODO: 2. GetMemoryMap() to get the MapKey
    // TODO: 3. ExitBootServices()
    // TODO: 4. Jump to Kernel Entry Point

    while (1) {
        __asm__ __volatile__("hlt");
    }

    return EFI_SUCCESS;
}
