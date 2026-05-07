ENTRY(_start)
SECTIONS {
    . = 1M;

    .boot : { *(.multiboot2) }

    .text ALIGN(4K) :
    {
        kernel_start = .;
        *(.text)
    }

    .rodata ALIGN(4K) : { *(.rodata) }
    .data   ALIGN(4K) : { *(.data) }

    .bss ALIGN(4K) :
    {
        *(COMMON)
        *(.bss)
        . = ALIGN(4K);
        kernel_end = .;
    }

    /DISCARD/ : { *(.eh_frame) *(.comment) }
}