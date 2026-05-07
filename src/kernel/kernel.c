#include "../cpu/idt.h"
#include "../drivers/vga.h"
#include "../drivers/timer.h"
#include "../shell/shell.h"
 
void kernel_main() {
 
    clear_screen();
 
    idt_init();
    timer_init();
    __asm__ volatile ("sti");
 
    print("booteaste el kernel.\n");
    print("bienvenido al os papa.\n");
    print("habemus shell.\n");
 
    prompt();
 
    for (;;) {
        __asm__ volatile ("hlt");
    }
}
 
