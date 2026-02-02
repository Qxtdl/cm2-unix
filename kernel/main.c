#include <kernel/device.h>
#include <kernel/tty.h>
#include <kernel/block.h>
#include <kernel/majors.h>
#include <kernel/proc.h>
#include <kernel/syscall.h>
#include <lib/stdlib.h>

dev_t tty0_devno;
struct device* tty0;
dev_t disk0_devno;

[[gnu::aligned(16)]] uint8_t init_thread_stack[64];

int8_t init_thread_exit;

void init_thread() {
    char test[] = "CM2-UNIX V0.2.1\nBooting...\n";
    char test1[] = "registered disk0 at 0xFFC3\n";
    init_thread_exit = 0;
    syscall(0, tty0_devno, (uint32_t) &test, sizeof(test) - 1);
    
    device_create(&disk0_devno, GEN_DISK_MAJOR, (void*) 0xFFC3);

    syscall(0, tty0_devno, (uint32_t) &test1, sizeof(test1) - 1);
    
    init_thread_exit = 1;
    syscall(4, 0, 0, 0); //exit()
}

[[gnu::aligned(16)]] uint8_t test_thread_stack[128];
char shell_name[] = "Shell v0.1.0\n";
char prompt[] = "# ";
char uname[] = "CM2-UNIX V0.2.1\n";
char bad_command[] = "-shell: bad cmd\n";

void test_thread() {
    
    //bodged pidwait
    while(init_thread_exit == 0) {
        syscall(3, 0, 0, 0); //yield()
    };

    //dev_write(tty0_devno, &test, sizeof(test)-1);
    syscall(0, tty0_devno, (uint32_t) &shell_name, sizeof(shell_name) - 1);
    
    while(1) {
        char buffer[16] = {0};
        //write prompt
        syscall(0, tty0_devno, (uint32_t) &prompt, sizeof(prompt) - 1);
        //read input
        int size = syscall(1, tty0_devno, (uint32_t) &buffer, sizeof(buffer));

        if (strncmp(buffer, "uname", size) == 0) {
            syscall(0, tty0_devno, (uint32_t) &uname, sizeof(uname) - 1);
        } else if (strncmp(buffer, "exit", size) == 0) {
            break;
        } else if (strncmp(buffer, "clear", size) == 0) {
            syscall(2, tty0_devno, TTY_IOCTL_CLEAR, 0);
        } else {
            syscall(0, tty0_devno, (uint32_t) &bad_command, sizeof(bad_command) - 1);
        }
    }

    //exit()
    syscall(4, 0, 0, 0);
}

void main() {
    *((uint8_t*) 0xFFF6) = 0;
    
    //initialize functions
    device_init();
    tty_init();
    gen_disk_init();
    proc_init();

    tty0 = device_create(&tty0_devno, TTY_MAJOR, (void*) 0xFFF1);
    
    proc_create((uint32_t) &init_thread, (uint32_t) &init_thread_stack + 64);
    proc_create((uint32_t) &test_thread, (uint32_t) &test_thread_stack + 64);

    //bootstrap the scheduler by getting the first process to run
    current_process = proc_dequeue();
    exit_kernel(); //jump into the current process
}


