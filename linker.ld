ENTRY(_start)

SECTIONS {
    . = 1M;

    .boot :
    {
        *(.multiboot2)
    }

    .text ALIGN(4K) :
    {
        *(.text)
    }

    .rodata ALIGN(4K) :
    {
        *(.rodata)
    }

    .data ALIGN(4K) :
    {
        *(.data)
    }

    .bss ALIGN(4K) :
    {
        *(COMMON)
        *(.bss)
        . = ALIGN(4K);
    }

    /DISCARD/ :
    {
        *(.eh_frame)
        *(.comment)
    }
}
