[org 0x7c00]
KERNEL_OFFSET equ 0x1000 ; The memory address where we load the kernel

    mov [BOOT_DRIVE], dl ; BIOS stores the boot drive in DL; save it

    mov bp, 0x9000       ; Set up the stack
    mov sp, bp

    call load_kernel     ; 1. Load C kernel from disk
    call switch_to_pm    ; 2. Switch to 32-bit mode

    jmp $

%include "gdt.asm"
%include "print.asm"

[bits 16]
load_kernel:
    mov bx, KERNEL_OFFSET ; Set buffer to 0x1000 (ES:BX)
    mov dh, 50            ; Read 15 sectors (plenty for our kernel)
    mov dl, [BOOT_DRIVE]  ; Select boot drive

    mov ah, 0x02          ; BIOS read sector function
    mov al, dh            ; Read DH sectors
    mov ch, 0x00          ; Cylinder 0
    mov dh, 0x00          ; Head 0
    mov cl, 0x02          ; Start reading from 2nd sector (sector 1 is bootloader)

    int 0x13              ; BIOS interrupt

    jc disk_error         ; Jump if Carry Flag is set (error)
    ret

disk_error:
    mov si, err_disk      ; Point SI to the error message
    call print            ; Use the 16-bit print function
    jmp $                 ; Lock up the CPU

err_disk db 'Error reading disk sectors!', 0x0d, 0x0a, 0

[bits 32]
begin_pm:
    call KERNEL_OFFSET    ; 3. Jump to the loaded kernel code
    jmp $

BOOT_DRIVE db 0

; Padding
times 510-($-$$) db 0
dw 0xaa55
