#include "efi.h"
#include "shared_types.h"

// Move these outside the function so they aren't on the stack
static framebuffer_info_t fb_info;
static font_t kernel_font;
static boot_info_t binfo;

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

    // Locate the APCI
    EFI_GUID acpi_guid = {0x8868e871, 0xe4f1, 0x11d3, {0xbc, 0x22, 0x00, 0x80, 0xc7, 0x3c, 0x88, 0x81}};
    void *rsdp = 0;
    for (uint64_t i = 0; i < SystemTable->NumberOfTableEntries; i++) {
        EFI_GUID *g = &SystemTable->ConfigurationTable[i].VendorGuid;
        if (g->Data1 == acpi_guid.Data1 && g->Data2 == acpi_guid.Data2 && g->Data3 == acpi_guid.Data3) {
            rsdp = SystemTable->ConfigurationTable[i].VendorTable;
            break;
        }
    }

    // 1. Initialize GOP
    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
    SystemTable->BootServices->HandleProtocol(SystemTable->ConsoleOutHandle, &gopGuid, (void **)&gop);

    fb_info.base_address = (uint32_t *)gop->Mode->FrameBufferBase;
    fb_info.buffer_size  = gop->Mode->FrameBufferSize;
    fb_info.width        = gop->Mode->Info->HorizontalResolution;
    fb_info.height       = gop->Mode->Info->VerticalResolution;
    fb_info.pitch        = gop->Mode->Info->PixelsPerScanLine;

    // Translate EFI pixel format so the kernel doesn't depend on efi.h
    switch (gop->Mode->Info->PixelFormat) {
        case PixelRedGreenBlueReserved8BitPerColor:
            fb_info.pixel_format = FB_PIXEL_RGBX;
            break;
        case PixelBlueGreenRedReserved8BitPerColor:
            fb_info.pixel_format = FB_PIXEL_BGRX;
            break;
        case PixelBitMask:
            fb_info.pixel_format  = FB_PIXEL_MASK;
            fb_info.pixel_mask_r  = gop->Mode->Info->PixelInformation.RedMask;
            fb_info.pixel_mask_g  = gop->Mode->Info->PixelInformation.GreenMask;
            fb_info.pixel_mask_b  = gop->Mode->Info->PixelInformation.BlueMask;
            break;
        default:
            fb_info.pixel_format = FB_PIXEL_UNKNOWN;
            break;
    }

    // 2. Locate File System
    EFI_GUID loaded_img_guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    EFI_GUID sfsp_guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
    EFI_LOADED_IMAGE_PROTOCOL *loaded_image;
    SystemTable->BootServices->HandleProtocol(ImageHandle, &loaded_img_guid, (void **)&loaded_image);
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs;
    SystemTable->BootServices->HandleProtocol(loaded_image->DeviceHandle, &sfsp_guid, (void **)&fs);

    // 3. Open Root
    EFI_FILE_PROTOCOL *root;
    fs->OpenVolume(fs, &root);

    // 4. Load kernel.bin
    EFI_FILE_PROTOCOL *kernel_file;
    root->Open(root, &kernel_file, u"kernel.bin", EFI_FILE_MODE_READ, 0);
    uint64_t kernel_addr = 0x100000;
    SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, 512, &kernel_addr);
    uint64_t read_size = 512 * 4096;
    kernel_file->Read(kernel_file, &read_size, (void *)kernel_addr);
    kernel_file->Close(kernel_file);

    // 5. Load font.psf (MUST HAPPEN BEFORE EXITING BOOT SERVICES)
    EFI_FILE_PROTOCOL *font_file;
    EFI_STATUS status = root->Open(root, &font_file, u"font.psf", EFI_FILE_MODE_READ, 0);
    if (status != EFI_SUCCESS) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, u"Error: font.psf not found.\r\n");
        while(1) __asm__ __volatile__("hlt");
    }
    uint64_t font_addr;
    SystemTable->BootServices->AllocatePages(AllocateAnyPages, EfiLoaderData, 4, &font_addr);
    uint64_t font_size = 4 * 4096;
    font_file->Read(font_file, &font_size, (void *)font_addr);
    font_file->Close(font_file);

    kernel_font.psf_header = (void*)font_addr;
    if (((uint8_t*)font_addr)[0] == PSF1_MAGIC0 && ((uint8_t*)font_addr)[1] == PSF1_MAGIC1) {
        kernel_font.glyph_buffer = (uint8_t*)font_addr + sizeof(psf1_header_t);
    } else {
        psf2_header_t *h2 = (psf2_header_t*)font_addr;
        kernel_font.glyph_buffer = (uint8_t*)font_addr + h2->header_size;
    }

    // 6. Final Prep for Exit
    // First call: map_size=0 just queries the required buffer size.
    uint64_t map_size = 0, map_key = 0, desc_size = 0;
    uint32_t desc_version = 0;
    SystemTable->BootServices->GetMemoryMap(&map_size, 0, &map_key, &desc_size, &desc_version);

    // Add one extra page — AllocatePages itself may add a new descriptor,
    // which would grow the map and invalidate the key.
    map_size += desc_size * 4;

    // AllocatePages returns a physical address into a uint64_t, not a pointer.
    // Keep them separate to avoid ABI confusion.
    uint64_t map_phys = 0;
    SystemTable->BootServices->AllocatePages(AllocateAnyPages, EfiLoaderData,
        (map_size / 4096) + 1, &map_phys);
    void *map_buffer = (void*)map_phys;

    // Second call: populate the buffer. map_key is now valid for ExitBootServices.
    // No Boot Services calls are allowed between this and ExitBootServices.
    SystemTable->BootServices->GetMemoryMap(&map_size, map_buffer, &map_key, &desc_size, &desc_version);

    // Exit ONCE — any failure here is unrecoverable
    SystemTable->BootServices->ExitBootServices(ImageHandle, map_key);

    // 7. Execute Kernel
    // No EFI calls are valid past this point.
    binfo.fb             = &fb_info;
    binfo.font           = &kernel_font;
    binfo.rsdp           = rsdp;
    binfo.mmap           = (memory_map_entry_t*)map_buffer;
    binfo.mmap_size      = map_size;
    binfo.mmap_desc_size = desc_size;

    void (*kernel_entry)(boot_info_t*) = (void (*)(boot_info_t*))kernel_addr;
    kernel_entry(&binfo);

    return EFI_SUCCESS;
}
