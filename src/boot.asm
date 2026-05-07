global _start
extern kernel_main

MB2_MAGIC    equ 0xE85250D6
MB2_ARCH     equ 0
MB2_LENGTH   equ header_end - header_start
MB2_CHECKSUM equ 0x100000000 - (MB2_MAGIC + MB2_ARCH + MB2_LENGTH)

section .multiboot2
align 8
header_start:
    dd MB2_MAGIC
    dd MB2_ARCH
    dd MB2_LENGTH
    dd MB2_CHECKSUM

    ; end tag
    dw 0
    dw 0
    dd 8
header_end:

section .bss
align 4096

pml4:
    resb 4096

pdpt:
    resb 4096

pdt:
    resb 4096

align 16
stack_bottom:
    resb 16384
stack_top:

section .rodata
gdt64:
    dq 0

.code equ $ - gdt64
    dw 0xFFFF
    dw 0
    db 0
    db 10011010b
    db 10101111b
    db 0

.data equ $ - gdt64
    dw 0xFFFF
    dw 0
    db 0
    db 10010010b
    db 00001111b
    db 0

.pointer:
    dw $ - gdt64 - 1
    dq gdt64

section .text
[BITS 32]

_start:
    cli

    ; multiboot magic check
    cmp eax, 0x36d76289
    jne .no_multiboot

    ; usar stack real
    mov esp, stack_top

    ; cargar GDT
    lgdt [gdt64.pointer]

    ; --------------------------
    ; limpiar page tables
    ; --------------------------

    mov edi, pml4
    mov ecx, 4096 / 4
    xor eax, eax
    rep stosd

    mov edi, pdpt
    mov ecx, 4096 / 4
    xor eax, eax
    rep stosd

    mov edi, pdt
    mov ecx, 4096 / 4
    xor eax, eax
    rep stosd

    ; --------------------------
    ; pml4[0] -> pdpt
    ; --------------------------

    mov eax, pdpt
    or eax, 0x3
    mov [pml4], eax

    ; --------------------------
    ; pdpt[0] -> pdt
    ; --------------------------

    mov eax, pdt
    or eax, 0x3
    mov [pdpt], eax

    ; --------------------------
    ; map first 2 MiB
    ; --------------------------

    mov eax, 0x000083
    mov [pdt], eax

    ; --------------------------
    ; map second 2 MiB
    ; --------------------------

    mov eax, 0x200083
    mov [pdt + 8], eax

    ; --------------------------
    ; enable PAE
    ; --------------------------

    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; --------------------------
    ; load PML4 into CR3
    ; --------------------------

    mov eax, pml4
    mov cr3, eax

    ; --------------------------
    ; enable long mode
    ; --------------------------

    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; --------------------------
    ; enable paging + protected mode
    ; --------------------------

    mov eax, cr0
    or eax, 1 << 31
    or eax, 1 << 0
    mov cr0, eax

    ; --------------------------
    ; jump to 64-bit mode
    ; --------------------------

    jmp 0x08:long_mode_entry

.no_multiboot:
    hlt

[BITS 64]

long_mode_entry:
    mov ax, 0x10

    mov ds, ax
    mov es, ax
    mov ss, ax

    mov rsp, stack_top
    mov rdi, rbx

    call kernel_main

.hang:
    hlt
    jmp .hang