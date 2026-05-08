#ifndef STRING_H
#define STRING_H

#include <stdint.h>

int strcmp(const char* a, const char* b);
void print_number(uint64_t n);

int    strlen(const char* s);
void   strncpy(char* dst, const char* src, int n);

#endif