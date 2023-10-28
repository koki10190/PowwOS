#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdint.h>

size_t strlen(const char *str);
size_t __strlen(const char *s);
size_t strnlen(const char *s, size_t len);
char *strcpy(char *dst, const char *src);
char *strcat(char *dst, const char *src);

int strncmp(const char *s1, const char *s2, size_t n);

int strcmp(const char *s1, const char *s2);

size_t strspn(const char *str, const char *accept);

size_t lfind(const char *str, const char accept);

size_t rfind(const char *str, const char accept);

char *strpbrk(const char *str, const char *accept);

char *strtok_r(char *str, const char *delim, char **saveptr);
int __strtoint(const char *str);
void reset_string_buffer(char *str, int size);

uint32_t int_count(int num);
void itoa(int num, char *number);
char *__itoa(int res);
char *__utoa(uint32_t res);

const char *to_string64(uint64_t value);
const char *to_hstring64(uint64_t value);
const char *to_hstring32(uint32_t value);
const char *to_hstring16(uint16_t value);
const char *to_hstring8(uint8_t value);

#endif