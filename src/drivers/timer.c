#include "timer.h"
#include "vga.h"
#include "../lib/io.h"
 
volatile uint64_t timer_ticks = 0;
volatile int sleeping = 0;
volatile uint64_t sleep_target = 0;
 
void timer_init() {
 
    uint16_t divisor = 1193180 / 100;
 
    outb(0x43, 0x36);
 
    outb(0x40, divisor & 0xFF);
    outb(0x40, divisor >> 8);
}
 
void timer_handler() {
 
    timer_ticks++;
 
    if (sleeping && timer_ticks >= sleep_target) {
 
        sleeping = 0;
        print("Canto el gallo che\n");
        render_prompt();
    }
}
 
void sleep(uint64_t ticks) {
 
    sleep_target = timer_ticks + ticks;
    sleeping = 1;
}
