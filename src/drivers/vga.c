#include "vga.h"
#include "../lib/io.h"
 
#define VGA_WIDTH  80
#define VGA_HEIGHT 25
 
static uint16_t* vga = (uint16_t*)0xB8000;
 
static int col = 0;
static int row = 0;
 
static uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}
 
void update_cursor() {
 
    uint16_t pos = row * VGA_WIDTH + col;
 
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
 
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}
 
void scroll() {
 
    for (int y = 1; y < VGA_HEIGHT; y++) {
 
        for (int x = 0; x < VGA_WIDTH; x++) {
 
            vga[(y - 1) * VGA_WIDTH + x] =
                vga[y * VGA_WIDTH + x];
        }
    }
 
    for (int x = 0; x < VGA_WIDTH; x++) {
 
        vga[(VGA_HEIGHT - 1) * VGA_WIDTH + x] =
            vga_entry(' ', 0x07);
    }
 
    row = VGA_HEIGHT - 1;
}
 
void clear_screen() {
 
    for (int y = 0; y < VGA_HEIGHT; y++) {
 
        for (int x = 0; x < VGA_WIDTH; x++) {
 
            vga[y * VGA_WIDTH + x] =
                vga_entry(' ', 0x07);
        }
    }
 
    row = 0;
    col = 0;
}
 
void putchar(char c) {
 
    /* newline */
    if (c == '\n') {
 
        col = 0;
        row++;
 
        if (row >= VGA_HEIGHT)
            scroll();
 
        update_cursor();
        return;
    }
 
    /* borrar */
    if (c == '\b') {
 
        if (col > 0) {
 
            col--;
 
            vga[row * VGA_WIDTH + col] =
                vga_entry(' ', 0x07);
        }
 
        update_cursor();
        return;
    }
 
    /* shawarma horizontal */
    if (col >= VGA_WIDTH) {
 
        col = 0;
        row++;
    }
 
    /* vertical scroll */
    if (row >= VGA_HEIGHT)
        scroll();
 
    vga[row * VGA_WIDTH + col] =
        vga_entry(c, 0x0A);
 
    col++;
    update_cursor();
}
 
void print(const char* str) {
 
    for (int i = 0; str[i] != '\0'; i++)
        putchar(str[i]);
}
 
void render_prompt() {
    print("\nxaeronOS> ");
}
