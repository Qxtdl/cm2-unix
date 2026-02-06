#include <kernel/proc.h>
#include <kernel/device.h>
#include <lib/stdlib.h>
#include <kernel/syscall.h>
#include <kernel/panic.h>
#include <stddef.h>

struct proc process_table[MAX_PROCESSES];

uint8_t free_processes[MAX_PROCESSES];
uint8_t free_processes_count;

struct proc* ready_queue[MAX_PROCESSES];
uint8_t ready_queue_head;
uint8_t ready_queue_tail;
uint8_t ready_queue_count;

struct proc* current_process;

uint32_t kernel_sp;

//add this process to the queue of processes ready to execute
int proc_enqueue(struct proc* process)
{
    if (ready_queue_count == MAX_PROCESSES) return -1;
    
    ready_queue[ready_queue_tail] = process;
    ready_queue_tail = (ready_queue_tail + 1) & MAX_PROCESSES_MSK;
    ready_queue_count++;
    
    return 0;
}

struct proc* proc_dequeue()
{
    if (ready_queue_count == 0) return NULL;
    
    struct proc* process = ready_queue[ready_queue_head];
    ready_queue_head = (ready_queue_head + 1) & MAX_PROCESSES_MSK;
    ready_queue_count--;
    
    return process;
}



void proc_init()
{
    ready_queue_head = 0;
    ready_queue_tail = 0;
    ready_queue_count = 0;
    current_process = NULL;

    free_processes_count = MAX_PROCESSES;


    for (int i = 0; i < MAX_PROCESSES; i++) {
        free_processes[i] = i;
        process_table[i].state = UNALLOCATED;
    }
}

struct proc* proc_create(uint32_t entry_point, uint32_t stack_pointer) {
    
    uint8_t tmp = free_processes_count;
    if (tmp == 0) return NULL;
    uint8_t index = free_processes[--tmp];
    struct proc* new_process = &process_table[index];
    free_processes_count = tmp;

    new_process->pid = index;
    new_process->return_address = entry_point;
    new_process->user_sp = stack_pointer;
    new_process->state = READY;
    new_process->return_value = 0;
    new_process->syscall_state = SYSCALL_STATE_NIL;
    
    proc_enqueue(new_process);
    return new_process;
}

void proc_delete(struct proc* process) {
    if (process->state == DEAD) {
        process->state = UNALLOCATED;
        free_processes[free_processes_count++] = process->pid;
    }
}

void proc_resume(struct proc* process, int return_value)
{
    process->state = READY;
    process->return_value = return_value;
    proc_enqueue(process);
    process->syscall_state = SYSCALL_STATE_NIL;
}


struct fd* proc_get_fd(int fd) {
    if (fd < 0 || fd > MAX_FD) {
        return NULL;
    }
    uint8_t fdnum = current_process->open_files[fd];
    if (fdnum > PROC_MAXFILES) {
        return NULL;
    }
    struct fd* descriptor = &fd_table[fdnum];

    return descriptor;
}

uint8_t proc_alloc_fd() {
    for (int i = 0; i < PROC_MAXFILES; i++) {
        if (current_process->open_files[i] == PROC_FILE_NIL) {
            return i;
        }
    }
    return PROC_FILE_NIL;
}




void proc_update()
{
    struct proc* process = current_process;
    
    //if the current process is ready we queue it up for execution
    if (process->state == READY && process != NULL) {
        proc_enqueue(process);
    }
    if (process->state == DEAD && process != NULL) {
        proc_delete(process);
    }

    if (free_processes_count == MAX_PROCESSES) { //out of processes
        panic("free_processes_count == MAX_PROCESSES");
    }

    //get a new process from the top of the queue
    struct proc* new_process = NULL;
    while(new_process == NULL) {
        device_update(); //do a kernel 'tick' basicly call all update functions
        syscall_update();
        new_process = proc_dequeue();
    }

    current_process = new_process;
    
}

