#include <stdint.h>
#include "idt.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

static uint16_t* vga = (uint16_t*)0xB8000;

static int col = 0;
static int row = 0;

/* ========================= */
/* IOO                  */
/* ========================= */

static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;

    __asm__ volatile (
        "inb %1, %0"
        : "=a"(ret)
        : "Nd"(port)
    );

    return ret;
}

/* ========================= */
/* VGA                       */
/* ========================= */

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

/* ========================= */
/* ilos                    */
/* ========================= */

int strcmp(const char* a, const char* b) {

    int i = 0;

    while (a[i] && b[i]) {

        if (a[i] != b[i])
            return 0;

        i++;
    }

    return a[i] == b[i];
}

/* ========================= */
/* SHELL                     */
/* ========================= */

char command_buffer[256];
int command_index = 0;

void prompt() {
    print("\nxaeronOS> ");
}

void execute_command() {

    command_buffer[command_index] = '\0';

    if (strcmp(command_buffer, "help")) {

        print("\nComandos disponibles:\n");
        print("help   - muestra esta ayuda\n");
        print("clear  - limpia la pantalla\n");
        print("about  - info del kernel\n");
        print("panic  - test exception\n");

    } else if (strcmp(command_buffer, "clear")) {

        clear_screen();

    } else if (strcmp(command_buffer, "about")) {

        print("\nxaeronOS");
        print("\nKernel 64-bit experimental");
        print("\nDesarollado por Bautista E. Luzardo");
        print("\nCon IRQs y teclado PS/2, azar y mujerzuelas\n");

    } else if (strcmp(command_buffer, "panic")) {

        __asm__ volatile ("int $0");

    } else if (command_index == 0) {

        /* gato no hace nada */

    } else {

        print("\nNo lo tengo a ese comando pa, fijate despues\n");
    }

    command_index = 0;

    prompt();
}

/* ========================= */
/* teclado                  */
/* ========================= */

char keyboard_map[128] = {
    0,
    27,
    '1','2','3','4','5','6','7','8','9','0',
    '-','=','\b',
    '\t',

    'q','w','e','r','t','y','u','i','o','p',
    '[',']','\n',

    0,

    'a','s','d','f','g','h','j','k','l',
    ';','\'','`',

    0,

    '\\',

    'z','x','c','v','b','n','m',
    ',','.','/',

    0,
    '*',
    0,
    ' '
};

void keyboard_handler() {

    uint8_t scancode = inb(0x60);

    /* soltar tecla */
    if (scancode & 0x80)
        return;

    if (scancode >= 128)
        return;

    char c = keyboard_map[scancode];

    if (!c)
        return;

    /* enter */
    if (c == '\n') {

        putchar('\n');

        execute_command();

        return;
    }

    /* borrar */
    if (c == '\b') {

        if (command_index > 0) {

            command_index--;

            putchar('\b');
        }

        return;
    }

    /* evitar overflow */
    if (command_index >= 255)
        return;

    command_buffer[command_index++] = c;

    putchar(c);
}


// KERNELLL


void kernel_main() {

    clear_screen();

    idt_init();

    __asm__ volatile ("sti");

    print("booteaste el kernel.\n");
    print("bienvenido al os papa.\n");
    print("habemus shell.\n");

    prompt();

    for (;;) {
        __asm__ volatile ("hlt");
    }
}