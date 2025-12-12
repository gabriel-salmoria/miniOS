[org 0x7c00]
KERNEL_OFFSET equ 0x1000 ; The memory address where we load the kernel

    mov [BOOT_DRIVE], dl ; BIOS stores the boot drive in DL; save it

    mov bp, 0x9000       ; Set up the stack
    mov sp, bp

    call load_kernel     ; 1. Load C kernel from disk
    call switch_to_pm    ; 2. Switch to 32-bit mode

    jmp $

%include "gdt.asm"       ; (See below for content)
%include "print.asm"     ; (Optional 16-bit print helpers)

[bits 16]
load_kernel:
    mov bx, KERNEL_OFFSET ; Destination address (ES:BX)
    mov dh, 15            ; Number of sectors to read (keep it simple)
    mov dl, [BOOT_DRIVE]
    
    mov ah, 0x02          ; BIOS read sector function
    mov al, dh            ; Read DH sectors
    mov ch, 0x00          ; Cylinder 0
    mov dh, 0x00          ; Head 0
    mov cl, 0x02          ; Start reading from 2nd sector (sector 1 is bootloader)
    
    int 0x13              ; BIOS interrupt
    jc disk_error         ; Jump if Carry Flag set (error)
    ret

disk_error:
    ; (Add error printing here if desired)
    jmp $

[bits 32]
begin_pm:
    call KERNEL_OFFSET    ; 3. Jump to the loaded kernel code
    jmp $

BOOT_DRIVE db 0

; Padding
times 510-($-$$) db 0
dw 0xaa55
