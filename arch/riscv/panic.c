#include <lib/stdlib.h>
#include <lib/hex.h>
#include <kernel/tty.h>
#include <kernel/proc.h>

#include <stdint.h>

void panic(void) {
    uint32_t sp;
    __asm__ volatile (
        "mv %0, sp"
        : "=r" (sp)
    );
    
    freestanding_tty_puts("KERNEL PANIC:\nSP $\n");
    freestanding_tty_puts(u32_to_hex(sp));
    freestanding_tty_puts("PROC:\nPID: $");
    freestanding_tty_puts(u32_to_hex(current_process->pid));
    freestanding_tty_puts("\nSYSCALL: $");
    freestanding_tty_puts(u32_to_hex(current_process->syscall_operation));
    
    while (1);
}
