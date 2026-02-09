#pragma once
#include <stdint.h>
#include <kernel/device.h>
#include <fs/vfs.h>

enum proc_state : uint8_t {
    BLOCKED,
    READY,
    DEAD,
    UNALLOCATED
};

typedef struct {
    struct device_request* req;
} dev_write_t;

typedef struct {
    uint8_t target_pid;
} waitpid_t;

typedef struct {
    path_walk_t walker;
} vfs_open_t;

typedef struct {
    fs_read_t fs;
} vfs_read_t;

typedef struct {
    fs_write_t fs;
} vfs_write_t;

typedef struct {
	path_walker_t walker;
	fs_read_t fs;
} exe_t;

#define SYSCALL_STATE_NIL 255
#define SYSCALL_STATE_BEGIN 0

#define PROC_MAXFILES 16
#define PROC_FILE_NIL 255

//this should be made cross platform but i dont care anymore
struct proc {
    uint32_t return_address;
    uint32_t user_sp;
    uint32_t saved_regs[12];
    uint32_t return_value;
    enum proc_state state;

    //this is for syscalls to remember how far they have progressed
    uint8_t syscall_state;
    uint8_t syscall_operation;
    uint8_t pid;
    
    //per process file descript table
    uint8_t open_files[PROC_MAXFILES];
    
    
    //this is the state that multi tick syscalls need
    union {
        dev_write_t dev_write_state;
        waitpid_t waitpid_state;
        vfs_open_t open_state;
        vfs_read_t read_state;
        vfs_write_t write_state;
        exec_t exec_state;
    };
};

typedef uint8_t pid_t;
#define MAX_PROCESSES 4
#define MAX_PROCESSES_MSK (MAX_PROCESSES - 1)
extern struct proc process_table[MAX_PROCESSES];

extern struct proc* current_process;

extern uint32_t kernel_sp;

int proc_enqueue(struct proc* process);
struct proc* proc_dequeue();
void proc_init();
struct proc* proc_create(uint32_t entry_point, uint32_t stack_pointer);
void proc_delete(struct proc* process);
void proc_resume(struct proc* process, int return_value);
struct fd* proc_get_fd(int fd);
uint8_t proc_alloc_fd();
void proc_update();



