[org 0x7c00]
KERNEL_OFFSET equ 0x1000

    mov [BOOT_DRIVE], dl

    ; [DEBUG] Announce we started
    mov bp, 0x9000
    mov sp, bp

    mov si, msg_boot
    call print_string

    call load_kernel

    mov si, msg_pm
    call print_string

    call switch_to_pm
    jmp $

%include "gdt.asm"

[bits 16]
load_kernel:
    mov bx, KERNEL_OFFSET
    mov dh, 50            ; Read 50 sectors
    mov dl, [BOOT_DRIVE]

    mov ah, 0x02          ; BIOS CHS Read
    mov al, dh
    mov ch, 0x00
    mov dh, 0x00
    mov cl, 0x02
    int 0x13

    jc disk_error         ; If Carry Flag=1, jump to error
    ret

disk_error:
    mov si, err_disk      ; Load error message
    call print_string     ; [FIX] Actually print it!
    jmp $

; --- New Print Routine ---
print_string:
    mov ah, 0x0e          ; BIOS TTY output
.loop:
    lodsb                 ; Load byte at SI into AL, increment SI
    cmp al, 0             ; Check for null terminator
    je .done
    int 0x10              ; Print char
    jmp .loop
.done:
    ret

msg_boot db 'Booting ShitOS...', 0x0d, 0x0a, 0
msg_pm   db 'Switching to PM...', 0x0d, 0x0a, 0
err_disk db 'Disk Read Error!', 0x0d, 0x0a, 0

[bits 32]
begin_pm:
    call KERNEL_OFFSET
    jmp $

BOOT_DRIVE db 0

times 510-($-$$) db 0
dw 0xaa55
