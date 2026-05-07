#include "../cpu/idt.h"
#include "../drivers/vga.h"
#include "../drivers/timer.h"
#include "../shell/shell.h"
#include "../mm/pmm.h"
#include "../mm/vmm.h"

void kernel_main(uint64_t mb2_info) {
    clear_screen();
    idt_init();
    timer_init();
    pmm_init(mb2_info);
    vmm_init();

    __asm__ volatile ("sti");

    print("booteaste el kernel.\n");
    print("bienvenido al os papa.\n");
    print("habemus shell.\n");
    prompt();

    for (;;) {
        __asm__ volatile ("hlt");
    }
}