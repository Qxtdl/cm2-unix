#include <stdint.h>

#include <kernel/panic.h>

uint32_t __stack_chk_guard = 0xDEADC0DE;

void __stack_chk_fail(void) {
    // uint32_t sp_value = 0;

    // __asm__ volatile (
    //     "mv %0, sp"
    //     : "=r" (sp_value)
    // );

    // TODO:
    // Move sp_value into a external register visible to the user?
    
    *((uint8_t *)0xFFF8) = 2;    // POST = FAIL
    *((uint8_t *)0xFFF7) = 0xFF; // FCAUSE = STACK CORRUPTED

    panic();
    while (1);
}