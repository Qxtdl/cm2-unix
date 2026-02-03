#include <kernel/syscall.h>
#include <kernel/device.h>
#include <kernel/panic.h>
#include <fs/vfs.h>
#include <stddef.h>

uint32_t syscall_args[4]; //arguments registers, we only need a0 to a3 right now

#define SYSCALL_COUNT 8

void (*syscall_setup_table[])() = {
    &dev_write,
    &dev_read,
    &dev_ioctl,
    &yield,
    &exit,
    &waitpid,
    &vfs_open,
    &vfs_read
};

void (*syscall_update_table[])(struct proc* process) = {
    &dev_write_update,
    &dev_read_update,
    NULL,
    NULL,
    NULL,
    &waitpid_update,
    &vfs_open_update,
    &vfs_read_update
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

    if (target_proc->state == UNALLOCATED) {
        process->state = READY;
        process->return_value = target_proc->return_value;
        proc_enqueue(process);
        process->syscall_state = SYSCALL_STATE_NIL;
    }
}

//hardcoded filesystem until i finish mount point resolution
extern struct superblock* romfs;

// int open(const char* path, uint8_t flags)
void vfs_open()
{
    struct proc* process = current_process;
    walk_path_init(&process->open_state.walker, (const char*) syscall_args[1]);
    process->open_state.walker.fs_state.fs = romfs;
    
    //block the process
    process->state = BLOCKED;
    process->syscall_state = SYSCALL_STATE_BEGIN;
}

void vfs_open_update(struct proc* process)
{
    uint8_t rt = walk_path(&process->open_state.walker);
    if (rt == 2) { //directory not found
        proc_resume(process, -1);
    } else if (rt == 1) { //we walked the entire path
        uint8_t new_fd = fd_alloc();
        struct fd* fd_p = &fd_table[new_fd];
        fd_p->file = process->open_state.walker.fs_state.dir;
        fd_p->flags = 0;
        fd_p->offset = 0;
        
        process->open_files[0] = new_fd;
        proc_resume(process, 0);
    }
}

//int write(int fd, void* buffer, uint32_t count)
void vfs_read()
{
    struct proc* process = current_process;
    //this is horrible lol, FIXME: add bounds checking
    process->read_state.fs.descriptor = &fd_table[process->open_files[syscall_args[1]]];
    process->read_state.fs.buffer = (void*) syscall_args[2];
    process->read_state.fs.count = syscall_args[3];
    process->read_state.fs.bytes_read = 0;
    process->read_state.fs.fs = romfs;
    process->read_state.fs.req = NULL;

    process->state = BLOCKED;
    process->syscall_state = SYSCALL_STATE_BEGIN;
}

void vfs_read_update(struct proc* process)
{
    int8_t rt = process->read_state.fs.fs->fops->read(&process->read_state.fs);
    if (rt != 0) {
        proc_resume(process, process->read_state.fs.bytes_read);
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

