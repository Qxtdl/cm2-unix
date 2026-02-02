#include <stdint.h>
#include <kernel/proc.h>

extern uint32_t syscall_args[4];

void dev_write();
void dev_write_update(struct proc* process);
void dev_read();
void dev_read_update(struct proc* process);
void dev_ioctl();
void yield();
void exit();
void waitpid();
void waitpid_update(struct proc* process);

//called from a thread, switches to kernel context
int syscall(uint32_t a, uint32_t b, uint32_t c, uint32_t d);

void syscall_update();

//called by the assembly syscall function
void process_syscall();

[[noreturn]] void exit_kernel();

