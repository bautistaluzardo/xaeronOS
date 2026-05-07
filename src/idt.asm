global idt_load
global isr0
extern exception_handler

section .text
[BITS 64]

idt_load:
    lidt [rdi]
    ret

isr0:
    cli

    mov rdi, 0
    call exception_handler

.hang:
    hlt
    jmp .hang