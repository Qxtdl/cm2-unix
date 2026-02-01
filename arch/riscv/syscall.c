#include <kernel/syscall.h>
#include <kernel/device.h>
#include <stddef.h>

uint32_t syscall_args[4]; //arguments registers, we only need a0 to a3 right now

#define SYSCALL_COUNT 1

void (*syscall_setup_table[])() = {
    &dev_write
};

void (*syscall_update_table[])(struct proc* process) = {
    &dev_write_update
};

//syscall from user process

//int dev_write(dev_t devno, void* buffer, uint32_t count)
void dev_write()
{
    struct proc* process = current_process;
    process->dev_req = device_newreq((void*) syscall_args[2], syscall_args[3], 0, DEVICE_OP_WR);
    
    struct device* dev = device_lookup(syscall_args[1]);
    device_queue_action(dev, process->dev_req);
    
    process->state = BLOCKED;
    process->syscall_state = SYSCALL_STATE_BEGIN;
}

void dev_write_update(struct proc* process)
{
    if (process->dev_req->state == DEVICE_STATE_FINISHED) {
        process->state = READY;
        process->return_value = process->dev_req->count;
        device_free_req(process->dev_req);
        proc_enqueue(process); //allow the process to be executed again
        process->syscall_state = SYSCALL_STATE_NIL;
    }
}

void syscall_update()
{
    struct proc* current = &process_table[0];
    struct proc* end = process_table + 4;
    
    while(current < end) {

        if (current->syscall_state != SYSCALL_STATE_NIL) {
            syscall_update_table[current->syscall_operation](current);
        }

        current++;
    }
}

void process_syscall()
{
    
    uint32_t syscallno = syscall_args[0];
    if (syscallno < SYSCALL_COUNT) {
        syscall_setup_table[syscallno]();
    } else {
        //bad syscall
    }
    //schedule new thread
    proc_update();

}

