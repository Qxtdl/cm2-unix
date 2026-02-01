#include <stdint.h>

const char *u32_to_hex(uint32_t value) {
    static const char lookup[] = "0123456789ABCDEF";
    static char buf[11];

    int i;
    for (i = 0; i < 8; i++) {
        uint8_t nibble = ((value >> (i * 4)) & 0xF);
        buf[i] = lookup[nibble];
    }
    buf[i + 1] = '\0';
    return buf;
}