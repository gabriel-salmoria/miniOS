[bits 64]
global jump_usermode

jump_usermode:
    cli

    ; USER_DS is 0x18. Plus RPL 3 = 0x1B
    mov ax, 0x1B
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Forge iretq stack frame
    push 0x1B        ; SS  (User Data Segment | RPL 3)
    push rsi         ; RSP (User Stack Pointer)
    push 0x202       ; RFLAGS (Interrupts enabled)
    push 0x23        ; CS  (User Code Segment | RPL 3 = 0x20 | 3 = 0x23)
    push rdi         ; RIP (User Entry Point)

    iretq
