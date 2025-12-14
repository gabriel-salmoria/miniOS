[bits 32]
global switch_task

; void switch_task(uint32_t *old_esp_ptr, uint32_t new_esp);
switch_task:
    ; 1. Save state
    pushf               ; <--- NEW: Save EFLAGS
    push ebx
    push esi
    push edi
    push ebp

    ; 2. Save current ESP into old_esp_ptr
    ; Offset is 24 because: 4 (ret) + 4 (eflags) + 16 (4 regs) = 24
    mov eax, [esp + 24]
    mov [eax], esp

    ; 3. Load new ESP
    ; Offset is 28 because args are 4 bytes further up
    mov esp, [esp + 28]

    ; 4. Restore state
    pop ebp
    pop edi
    pop esi
    pop ebx
    popf                ; <--- NEW: Restore EFLAGS (Enables Interrupts!)

    ret
