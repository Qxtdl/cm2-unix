#include <kernel/syscall.h>
#include <kernel/device.h>
#include <fs/vfs.h>
#include <stddef.h>

uint32_t syscall_args[4]; //arguments registers, we only need a0 to a3 right now


//syscall from user process

void process_syscall()
{
    
    uint32_t syscallno = syscall_args[0];
   
    //int write(dev_t devno, void* buffer, uint32_t count)
    if (syscallno == 0) {
        dev_write(syscall_args[1], (void*) syscall_args[2], syscall_args[3]);
    }
   
    //schedule new thread
    proc_update();

}

