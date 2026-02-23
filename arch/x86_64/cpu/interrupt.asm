[bits 64]
extern isr_handler

%macro ISR_NOERRCODE 1
    global isr%1
    isr%1:
        push qword 0      ; Dummy error code
        push qword %1     ; Interrupt number
        jmp isr_common_stub
%endmacro

%macro ISR_ERRCODE 1
    global isr%1
    isr%1:
        push qword %1     ; Interrupt number
        jmp isr_common_stub
%endmacro

; --- GENERATE HANDLERS ---
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_ERRCODE   17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_ERRCODE   30
ISR_NOERRCODE 31
ISR_NOERRCODE 32 ; IRQ 0 (Timer)
ISR_NOERRCODE 33 ; IRQ 1 (Keyboard)
ISR_NOERRCODE 34 ; IRQ 2
ISR_NOERRCODE 35 ; IRQ 3
ISR_NOERRCODE 36 ; IRQ 4
ISR_NOERRCODE 37 ; IRQ 5
ISR_NOERRCODE 38 ; IRQ 6
ISR_NOERRCODE 39 ; IRQ 7
ISR_NOERRCODE 40 ; IRQ 8
ISR_NOERRCODE 41 ; IRQ 9
ISR_NOERRCODE 42 ; IRQ 10
ISR_NOERRCODE 43 ; IRQ 11
ISR_NOERRCODE 44 ; IRQ 12
ISR_NOERRCODE 45 ; IRQ 13
ISR_NOERRCODE 46 ; IRQ 14
ISR_NOERRCODE 47 ; IRQ 15

; --- THE STUB TABLE ---
global isr_stub_table
isr_stub_table:
    %assign i 0
    %rep    48
        dq isr%+i
        %assign i i+1
    %endrep

; --- COMMON HANDLER ---
extern isr_handler

extern isr_handler

isr_common_stub:
    ; Save all registers
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; --- NEW: Align Stack to 16-bytes ---
    mov rbp, rsp          ; Save original RSP
    and rsp, -16          ; Align down to 16-byte boundary

    mov rdi, rbp          ; Pass the original stack (registers_t*) as 1st argument
    call isr_handler

    mov rsp, rbp          ; Restore original stack
    ; ------------------------------------

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    add rsp, 16
    iretq
