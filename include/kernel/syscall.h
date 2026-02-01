#include <stdint.h>
#include <kernel/proc.h>

extern uint32_t syscall_args[4];

//called from a thread, switches to kernel context
void syscall(uint32_t a, uint32_t b, uint32_t c, uint32_t d);

//called by the assembly syscall function, never returns
void process_syscall();

[[noreturn]] void exit_kernel();

