[bits 16]

print:
    mov ah, 0x0e        ; BIOS TTY output
.loop:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    ret

print_hex:
    ; Optional: minimal hex printer if you need debugging later
    mov ah, 0x0e
    mov al, '?'         ; Placeholder
    int 0x10
    ret
