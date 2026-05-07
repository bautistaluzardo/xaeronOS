#ifndef VGA_H
#define VGA_H
 
#include <stdint.h>
 
void update_cursor();
void scroll();
void clear_screen();
void putchar(char c);
void print(const char* str);
void render_prompt();
void print_hex(uint64_t value);
 
#endif
 
