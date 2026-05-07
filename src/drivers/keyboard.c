#include "keyboard.h"
#include "vga.h"
#include "timer.h"
#include "../lib/io.h"
#include "../shell/shell.h"

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

    if (sleeping)
        return;

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