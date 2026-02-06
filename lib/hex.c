#include <stdint.h>

const char *u32_to_hex(uint32_t value) {
    static const char lookup[] = "0123456789ABCDEF";
    static char buf[9];
    for (int i = 0; i < 8; i++) {
        buf[7 - i] = lookup[value & 0xf];
        value >>= 4;
    }
    buf[8] = '\0';
    return buf;
}