bits 32
global sys_call_isr
extern sys_call

sys_call_isr:
    pusha

    push ds
    push es
    push fs
    push gs

    push esp
    call sys_call
    add esp, 4

    mov esp, eax

    pop gs
    pop fs
    pop es
    pop ds

    popa

    iretd
