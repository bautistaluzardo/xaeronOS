#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

extern volatile uint64_t timer_ticks;
extern volatile int sleeping;
extern volatile uint64_t sleep_target;

void timer_init();
void timer_handler();
void sleep(uint64_t ticks);

#endif