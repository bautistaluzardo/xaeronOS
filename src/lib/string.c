#include "string.h"
#include "../drivers/vga.h"

int strcmp(const char* a, const char* b) {

    int i = 0;

    while (a[i] && b[i]) {

        if (a[i] != b[i])
            return 0;

        i++;
    }

    return a[i] == b[i];
}

void print_number(uint64_t n) {

    char buffer[32];

    int i = 0;

    if (n == 0) {
        putchar('0');
        return;
    }

    while (n > 0) {

        buffer[i++] = '0' + (n % 10);

        n /= 10;
    }

    while (i > 0)
        putchar(buffer[--i]);
}

int strlen(const char* s) {
    int i = 0;
    while (s[i]) i++;
    return i;
}

void strncpy(char* dst, const char* src, int n) {
    int i = 0;
    while (i < n && src[i]) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}