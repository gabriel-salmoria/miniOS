#include "efi.h"
#include "shared_types.h"

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

    // 1. Initialize GOP
    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
    SystemTable->BootServices->HandleProtocol(SystemTable->ConsoleOutHandle, &gopGuid, (void **)&gop);

    framebuffer_info_t fb_info;
    fb_info.base_address = (uint32_t *)gop->Mode->FrameBufferBase;
    fb_info.buffer_size  = gop->Mode->FrameBufferSize;
    fb_info.width        = gop->Mode->Info->HorizontalResolution;
    fb_info.height       = gop->Mode->Info->VerticalResolution;
    fb_info.pitch        = gop->Mode->Info->PixelsPerScanLine;

    // 2. Locate File System
    EFI_GUID loaded_img_guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    EFI_GUID sfsp_guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

    EFI_LOADED_IMAGE_PROTOCOL *loaded_image;
    SystemTable->BootServices->HandleProtocol(ImageHandle, &loaded_img_guid, (void **)&loaded_image);

    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs;
    SystemTable->BootServices->HandleProtocol(loaded_image->DeviceHandle, &sfsp_guid, (void **)&fs);

    // 3. Open Root and read kernel.bin
    EFI_FILE_PROTOCOL *root;
    fs->OpenVolume(fs, &root);

    EFI_FILE_PROTOCOL *kernel_file;
    EFI_STATUS status = root->Open(root, &kernel_file, u"kernel.bin", EFI_FILE_MODE_READ, 0);
    if (status != EFI_SUCCESS) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, u"Error: kernel.bin not found.\r\n");
        while(1) __asm__ __volatile__("hlt");
    }

    // Allocate 2MB at 0x100000
    uint64_t kernel_addr = 0x100000;
    SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, 512, &kernel_addr);

    uint64_t read_size = 512 * 4096;
    kernel_file->Read(kernel_file, &read_size, (void *)kernel_addr);
    kernel_file->Close(kernel_file);
    root->Close(root);

    // 4. Get Memory Map and Exit
    uint64_t map_size = 0, map_key = 0, desc_size = 0;
    uint32_t desc_version = 0;
    SystemTable->BootServices->GetMemoryMap(&map_size, 0, &map_key, &desc_size, &desc_version);

    map_size += 4096; // Buffer room
    void *map_buffer;
    SystemTable->BootServices->AllocatePages(AllocateAnyPages, EfiLoaderData, (map_size / 4096) + 1, (uint64_t*)&map_buffer);
    SystemTable->BootServices->GetMemoryMap(&map_size, map_buffer, &map_key, &desc_size, &desc_version);

    SystemTable->BootServices->ExitBootServices(ImageHandle, map_key);

    // 5. Execute Kernel
    void (*kernel_entry)(framebuffer_info_t*) = (void (*)(framebuffer_info_t*))kernel_addr;
    kernel_entry(&fb_info);

    return EFI_SUCCESS;
}
