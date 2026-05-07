global idt_load
global isr0
global irq1
global irq0
global isr14
extern timer_handler
extern keyboard_handler
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

irq1:
    cli

    ; guardar registros generales
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

    ; stack align para ABI SysV
    sub rsp, 8

    call keyboard_handler

    add rsp, 8

    ; EOI al PIC
    mov al, 0x20
    out 0x20, al

    ; restaurar registros
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

    iretq

irq0:
    cli

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

    sub rsp, 8

    call timer_handler

    add rsp, 8

    mov al, 0x20
    out 0x20, al

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

    iretq

isr14:
    cli
    push 14
    call exception_handler
    hlt