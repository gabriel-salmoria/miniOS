[bits 64]
global syscall_entry
extern syscall_handler

syscall_entry:
    swapgs
    mov gs:[0], rsp       ; Save User RSP temporarily
    mov rsp, gs:[8]       ; Load Kernel RSP

    ; Push exactly 8 registers (64 bytes) to ensure 16-byte ABI alignment
    push qword gs:[0]     ; 1. Push User RSP so we can POP it later
    push r11              ; 2. Save RFLAGS
    push rcx              ; 3. Save User RIP
    push rdx              ; 4. Save Arg3
    push rsi              ; 5. Save Arg2
    push rdi              ; 6. Save Arg1
    push rax              ; 7. Save Syscall ID
    push qword 0          ; 8. CRITICAL: Dummy push to force 16-byte alignment!

    ; Shuffle registers for the C calling convention
    mov rcx, rdx
    mov rdx, rsi
    mov rsi, rdi
    mov rdi, rax

    call syscall_handler

    ; Skip the dummy value (8 bytes) and the saved RAX (8 bytes)
    add rsp, 16

    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop r11
    pop rsp               ; Robustly restore User RSP directly from the stack

    swapgs
    o64 sysret
