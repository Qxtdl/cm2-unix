#include <stdint.h>
#include <stdarg.h>
#include <arch/riscv/memorymap.h>

#include <lib/hex.h>

void kputc(char c)
{
    if (c == '\n')
    {
        *TTY_LOC = ((*TTY_LOC + 32) & 0b11100000);
        *TTY_CHAR = c;
        return;
    }
    else (*TTY_LOC)++;;
    *TTY_CHAR = c;
    *TTY_WRITE = 1;
}

void kputs(const char *str)
{
    uint8_t i = 0;
    while (str[i] != '\0')
    {
        if (str[i] == '\n' || *TTY_LOC == 255)
        {
            *TTY_LOC = ((*TTY_LOC + 32) & 0b11100000);
        }
        else (*TTY_LOC)++;;
        *TTY_CHAR = str[i++];
        *TTY_WRITE = 1;
    }
}

void kprintf(const char *fmt, ...) 
{
    va_list params;
    va_start(params, fmt);

    while (*fmt != '\0') {
        if (*fmt != '%') kputc(*fmt);
        else {
            fmt++;

            if (*fmt == 'c') {
                char ch = va_arg(params, int);
                kputc(ch);
            }
            else if (*fmt == 's') {
                char *s = va_arg(params, char *);
                kputs(s);
            }
            else if (*fmt == 'x') {
                int num = va_arg(params, int);
                char *hex = u32_to_hex(num);
                kputs(hex);
            }
        }
        fmt++;
    }
}