[bits 32]

; Define a macro for ISRs without error code (pushes dummy 0)
%macro ISR_NOERRCODE 1
    global isr%1
    isr%1:
        push byte 0    ; Dummy error code
        push byte %1   ; Interrupt number
        jmp isr_common_stub
%endmacro

; Define a macro for ISRs with error code
%macro ISR_ERRCODE 1
    global isr%1
    isr%1:
        push byte %1   ; Interrupt number
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
    %rep    48      ; CHANGE THIS FROM 32 TO 48
        dd isr%+i
        %assign i i+1
    %endrep

; --- COMMON HANDLER ---
extern isr_handler

isr_common_stub:
    pusha           ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

    mov ax, ds      ; Lower 16-bits of eax = ds.
    push eax        ; Save the data segment descriptor

    mov ax, 0x10    ; Load the kernel data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call isr_handler

    pop eax         ; Reload the original data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa            ; Pops edi,esi,ebp...
    add esp, 8      ; Cleans up the pushed error code and ISR number
    iret            ; Pops CS, EIP, EFLAGS, SS, ESP
