[bits 64]
global jump_usermode

; rdi = User RIP (Entry point)
; rsi = User RSP (User Stack)
jump_usermode:
    cli

    ; Data Segment selector (USER_DS | RPL 3 = 0x20 | 3 = 0x23)
    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Forge iretq stack frame
    push 0x23        ; SS  (User Data Segment | RPL 3)
    push rsi         ; RSP (User Stack Pointer)
    push 0x202       ; RFLAGS (Interrupts enabled)
    push 0x1B        ; CS  (User Code Segment | RPL 3 = 0x18 | 3 = 0x1B)
    push rdi         ; RIP (User Entry Point)

    iretq
