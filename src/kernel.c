#include <stdint.h>

#define VGA_MEMORY (uint16_t*)0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

static uint16_t* vga = (uint16_t*)0xB8000;
static int col = 0;
static int row = 0;

static uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t)c | (uint16_t)color << 8;
}

void clear_screen() {
    for (int y = 0; y < VGA_HEIGHT; y++)
        for (int x = 0; x < VGA_WIDTH; x++)
            vga[y * VGA_WIDTH + x] = vga_entry(' ',0x07);
}

void putchar(char c) {
    if (c == '\n') {
        col = 0;
        row++;
        return;
    }
    vga[row * VGA_WIDTH + col] = vga_entry(c, 0x0A);
    col++;
}

void print(const char* str) {
    for (int i = 0; str[i] != '\0'; i++)
        putchar(str[i]);
}

void kernel_main() {
    clear_screen();
    print("booteaste el kernel.\n");
    print("bienvenido al os papa.\n");

    for(;;) __asm__("hlt");
}
