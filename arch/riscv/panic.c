#include <lib/stdlib.h>
#include <lib/hex.h>
#include <kernel/tty.h>

#include <stdint.h>

void panic(void) {
    uint32_t sp;
    __asm__ volatile (
        "mv %0, sp"
        : "=r" (sp)
    );
    
    freestanding_tty_puts("KERNEL PANIC:\nSP $");
    freestanding_tty_puts(u32_to_hex(sp));
    
    while (1);
}