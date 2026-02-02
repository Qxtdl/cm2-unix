#include <kernel/syscall.h>
#include <kernel/device.h>
#include <kernel/panic.h>
#include <stddef.h>

uint32_t syscall_args[4]; //arguments registers, we only need a0 to a3 right now

#define SYSCALL_COUNT 6

void (*syscall_setup_table[])() = {
    &dev_write,
    &dev_read,
    &dev_ioctl,
    &yield,
    &exit,
    &waitpid
};

void (*syscall_update_table[])(struct proc* process) = {
    &dev_write_update,
    &dev_read_update,
    NULL,
    NULL,
    NULL,
    &waitpid_update
};

//syscall from user process

//int dev_write(dev_t devno, void* buffer, uint32_t count)
void dev_write()
{
    struct proc* process = current_process;
    process->dev_write_state.req = device_newreq((void*) syscall_args[2], syscall_args[3], 0, DEVICE_OP_WR);
    
    struct device* dev = device_lookup(syscall_args[1]);
    device_queue_action(dev, process->dev_write_state.req);
    
    process->state = BLOCKED;
    process->syscall_state = SYSCALL_STATE_BEGIN;
}

void dev_write_update(struct proc* process)
{
    if (process->dev_write_state.req->state == DEVICE_STATE_FINISHED) {
        process->state = READY;
        process->return_value = process->dev_write_state.req->count;
        device_free_req(process->dev_write_state.req);
        proc_enqueue(process); //allow the process to be executed again
        process->syscall_state = SYSCALL_STATE_NIL;
    }
}

//int dev_read(dev_t devno, void* buffer, uint32_t count)
void dev_read()
{
    struct proc* process = current_process;
    process->dev_write_state.req = device_newreq((void*) syscall_args[2], syscall_args[3], 0, DEVICE_OP_RD);
    
    struct device* dev = device_lookup(syscall_args[1]);
    device_queue_action(dev, process->dev_write_state.req);
    
    process->state = BLOCKED;
    process->syscall_state = SYSCALL_STATE_BEGIN;
}

void dev_read_update(struct proc* process)
{
    if (process->dev_write_state.req->state == DEVICE_STATE_FINISHED) {
        process->state = READY;
        process->return_value = process->dev_write_state.req->count;
        device_free_req(process->dev_write_state.req);
        proc_enqueue(process); //allow the process to be executed again
        process->syscall_state = SYSCALL_STATE_NIL;
    }
}

//int dev_ioctl(dev_t devno, int cmd, void* arg)
void dev_ioctl() {
    struct device* dev = device_lookup(syscall_args[1]);
    current_process->return_value = dev->ops->ioctl(dev, syscall_args[2], (void*) syscall_args[3]);
}

void yield()
{
    //yield does nothing lol
}

//void exit(int return_code)
void exit()
{
    current_process->state = DEAD;
    current_process->return_value = syscall_args[1];
}

//int waitpid(pid_t pid)
void waitpid()
{
    current_process->state = BLOCKED;
    current_process->waitpid_state.target_pid = syscall_args[1];
    current_process->syscall_state = SYSCALL_STATE_BEGIN;
}

void waitpid_update(struct proc* process)
{
    struct proc* target_proc = &process_table[process->waitpid_state.target_pid];

    if (target_proc->state == DEAD) {
        process->state = READY;
        process->return_value = target_proc->return_value;
        proc_enqueue(process);
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
        current_process->syscall_operation = syscallno;
        syscall_setup_table[syscallno]();
    } else {
        //bad syscall
    }
    //schedule new thread
    proc_update();

}

