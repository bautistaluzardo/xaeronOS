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
    mov edi, ebx
    cmp eax, 0x36d76289
    jne .no_multiboot
    mov esp, 0x10a000

    db 0x0F, 0x01, 0x15
    dd 0x102018

    ; pml4[0] = pdpt | 3
    mov eax, 0x104003
    mov ebx, 0x103000
    mov dword [ebx], eax

    ; pdpt[0] = pdt | 3
    mov eax, 0x105003
    mov ebx, 0x104000
    mov dword [ebx], eax

    ; pdt[0] = first 2MB
    mov eax, 0x000083
    mov ebx, 0x105000
    mov dword [ebx], eax

    ; pdt[1] = second 2MB
    mov eax, 0x200083
    mov ebx, 0x105008
    mov dword [ebx], eax

    ; enable PAE
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; load pml4 into cr3
    mov eax, 0x103000
    mov cr3, eax

    ; enable long mode in EFER
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; enable paging and protected mode
    mov eax, cr0
    or eax, 1 << 31
    or eax, 1 << 0
    mov cr0, eax

    ; far jump to long mode - address filled after first build
    db 0xEA
    dd 0x0010107b      ; placeholder - we fill this after objdump
    dw 0x08

.no_multiboot:
    hlt

[BITS 64]
long_mode_entry:
    mov ax, 0x10
    mov ss, ax
    mov ds, ax
    mov es, ax
    call kernel_main
    hlt
