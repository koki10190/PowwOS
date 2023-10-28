#include <string.h>
#include <stdint.h>
#include <uart.h>

size_t strlen(const char *str) {
    /** The length of the string to be output. */
    size_t len = 0;
    while (str[len]) {
        len++;
    }

    return len;
}

size_t strnlen(const char *s, size_t len) {
    size_t cnt = 0;
    while (cnt < len && *s++ != '\0') {
        cnt++;
    }
    return cnt;
}

char *strcpy(char *dst, const char *src) {
    char *p = dst;
    while ((*p++ = *src++) != '\0')
        /* */;
    return dst;
}

size_t __strlen(const char *s) {
    size_t cnt = 0;
    while (*s++ != '\0') {
        cnt++;
    }
    return cnt;
}

char *strcat(char *dst, const char *src) {
    return strcpy(dst + __strlen(dst), src);
}

int strncmp(const char *s1, const char *s2, size_t n) {
    unsigned int count = 0;
    while (count < n) {
        if (s1[count] == s2[count]) {
            if (s1[count] == '\0')
                return 0;
            else
                count++;
        } else
            return s1[count] - s2[count];
    }

    return 0;
}

int strcmp(const char *s1, const char *s2) {
    if (__strlen(s1) != __strlen(s2))
        return s2 - s1;
    return strncmp(s1, s2, __strlen(s1));
}

size_t strspn(const char *str, const char *accept) {
    const char *ptr = str;
    const char *acc;

    while (*str) {
        for (acc = accept; *acc; ++acc) {
            if (*str == *acc) {
                break;
            }
        }
        if (*acc == '\0') {
            break;
        }

        str++;
    }

    return str - ptr;
}

size_t lfind(const char *str, const char accept) {
    size_t i = 0;
    while (str[i] != accept) {
        i++;
    }
    return (size_t)(str) + i;
}

size_t rfind(const char *str, const char accept) {
    size_t i = __strlen(str) - 1;
    while (str[i] != accept) {
        if (i == 0)
            return 4294967295;
        i--;
    }
    return (size_t)(str) + i;
}

char *strpbrk(const char *str, const char *accept) {
    const char *acc = accept;

    if (!*str) {
        return NULL;
    }

    while (*str) {
        for (acc = accept; *acc; ++acc) {
            if (*str == *acc) {
                break;
            }
        }
        if (*acc) {
            break;
        }
        ++str;
    }

    if (*acc == '\0') {
        return NULL;
    }

    return (char *)str;
}

char *strtok_r(char *str, const char *delim, char **saveptr) {
    char *token;
    if (str == NULL) {
        str = *saveptr;
    }
    str += strspn(str, delim);
    if (*str == '\0') {
        *saveptr = str;
        return NULL;
    }
    token = str;
    str = strpbrk(token, delim);
    if (str == NULL) {
        *saveptr = (char *)lfind(token, '\0');
    } else {
        *str = '\0';
        *saveptr = str + 1;
    }
    return token;
}

int __strtoint(const char *str) {
    int value = 0;

    for (int counter = __strlen(str) - 1, multiplier = 1; !(counter < 0); --counter, multiplier *= 10) {
        value += (str[counter] - 0x30) * multiplier;
    }

    return value;
}

uint32_t int_count(int num) {
    uint32_t count = 0;
    if (num == 0)
        return 1;
    while (num > 0) {
        count++;
        num = num / 10;
    }
    return count;
}

void itoa(int num, char *number) {
    int count = int_count(num);
    int __i = count - 1;
    char ch;
    if (num == 0 && count == 1) {
        number[0] = '0';
        number[1] = '\0';
    } else {
        while (num != 0) {
            ch = num % 10;
            number[__i] = ch + '0';
            __i--;
            num = num / 10;
        }
        number[count] = '\0';
    }
}
char *__itoa(int res) {
    int size = 0;
    int t = res;

    while (t / 10 != 0) {
        t = t / 10;
        size++;
    }
    static char ret[128];
    size++;
    ret[size] = '\0';
    t = res;
    int i = size - 1;
    while (i >= 0) {
        ret[i] = (t % 10) + '0';
        t = t / 10;
        i--;
    }

    return ret;
}

char *__utoa(uint32_t res) {
    uint32_t size = 0;
    uint32_t t = res;

    while (t / 10 != 0) {
        t = t / 10;
        size++;
    }
    static char ret[64];
    size++;
    ret[size] = '\0';
    t = res;
    uint32_t i = size - 1;
    while (i >= 0) {
        ret[i] = (t % 10) + '0';
        t = t / 10;
        i--;
    }

    return ret;
}

char *uint64_to_string(uint64_t value) {
    static char uint64_to_string_output[128] = {'0'};
    uint8_t size;
    uint64_t size_test = value;

    while (size_test / 10 > 0) {
        size_test /= 10;
        size++;
    }

    uint8_t index = 0;
    while (value / 10 > 0) {
        uint8_t remainder = value % 10;
        value /= 10;
        uint64_to_string_output[size - index] = remainder + '0';
        uart_putchar(uint64_to_string_output[size - index]);
        index++;
    }
    uint8_t remainder = value % 10;
    uint64_to_string_output[size - index] = remainder + '0';
    uint64_to_string_output[size + 1] = 0;
    uart_puts("\n");

    return uint64_to_string_output;
}

char hexTo_StringOutput[128];
const char *to_hstring64(uint64_t value) {
    uint64_t *valPtr = &value;
    uint8_t *ptr;
    uint8_t tmp;
    uint8_t size = 8 * 2 - 1;
    for (uint8_t i = 0; i < size; i++) {
        ptr = ((uint8_t *)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexTo_StringOutput[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        hexTo_StringOutput[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    hexTo_StringOutput[size + 1] = 0;
    return hexTo_StringOutput;
}

char hexTo_StringOutput32[128];
const char *to_hstring32(uint32_t value) {
    uint32_t *valPtr = &value;
    uint8_t *ptr;
    uint8_t tmp;
    uint8_t size = 4 * 2 - 1;
    for (uint8_t i = 0; i < size; i++) {
        ptr = ((uint8_t *)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexTo_StringOutput32[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        hexTo_StringOutput32[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    hexTo_StringOutput32[size + 1] = 0;
    return hexTo_StringOutput32;
}

char hexTo_StringOutput16[128];
const char *to_hstring16(uint16_t value) {
    uint16_t *valPtr = &value;
    uint8_t *ptr;
    uint8_t tmp;
    uint8_t size = 2 * 2 - 1;
    for (uint8_t i = 0; i < size; i++) {
        ptr = ((uint8_t *)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexTo_StringOutput16[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        hexTo_StringOutput16[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    hexTo_StringOutput16[size + 1] = 0;
    return hexTo_StringOutput16;
}

char hexTo_StringOutput8[128];
const char *to_hstring8(uint8_t value) {
    uint8_t *valPtr = &value;
    uint8_t *ptr;
    uint8_t tmp;
    uint8_t size = 1 * 2 - 1;
    for (uint8_t i = 0; i < size; i++) {
        ptr = ((uint8_t *)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexTo_StringOutput8[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        hexTo_StringOutput8[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    hexTo_StringOutput8[size + 1] = 0;
    return hexTo_StringOutput8;
}

char uintTo_StringOutput[128];
const char *to_string64(uint64_t value) {
    for (int i = 0; i < 128; i++) {
        uintTo_StringOutput[i] = '\0';
    }
    uint8_t size;
    uint64_t sizeTest = value;
    while (sizeTest / 10 > 0) {
        sizeTest /= 10;
        size++;
    }

    uint8_t index = 0;
    while (value / 10 > 0) {
        uint8_t remainder = value % 10;
        value /= 10;
        uintTo_StringOutput[size - index] = remainder + '0';
        index++;
    }
    uint8_t remainder = value % 10;
    uintTo_StringOutput[size - index] = remainder + '0';
    uintTo_StringOutput[size + 1] = 0;
    return uintTo_StringOutput;
}

void reset_string_buffer(char *str, int size) {
    for (int i = 0; i < 30; i++)
        str[i] = '\0';
}