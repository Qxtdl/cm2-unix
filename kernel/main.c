#include <kernel/device.h>
#include <kernel/tty.h>
#include <kernel/block.h>
#include <kernel/majors.h>
#include <kernel/proc.h>
#include <kernel/syscall.h>
#include <lib/stdlib.h>

#include <kernel/tilegpu.h>

dev_t tty0_devno;
struct device* tty0;

dev_t disk0_devno;

dev_t gpu0_devno;
struct device* gpu0;

[[gnu::aligned(16)]] uint8_t init_thread_stack[64];


void init_thread() {
    const char *test = "CM2-UNIX V0.2.3\nBooting...\n";
    const char *test1 = "registered disk0 at 0xFFC3\n";



    
    // const char *tilegpu_print = "tilegpu\ntest";
    // syscall(DEV_WRITE, gpu0_devno, (uint32_t) tilegpu_print, strnlen(tilegpu_print, 32));

    syscall(DEV_WRITE, tty0_devno, (uint32_t) test, strnlen(test, 32));
    
    device_create(&disk0_devno, GEN_DISK_MAJOR, (void*) 0xFFC3);

    syscall(DEV_WRITE, tty0_devno, (uint32_t) test1, strnlen(test1, 32));

    syscall(EXIT, 0, 0, 0); //exit(0)
}

[[gnu::aligned(16)]] uint8_t test_thread_stack[128];
char shell_name[] = "Shell v0.1.1\n";
char prompt[] = "# ";
char uname[] = "CM2-UNIX V0.2.3\nbuild: " __DATE__ "\n";
char bad_command[] = "-shell: bad cmd\n";

void test_thread() {
    //waitpid(3);
    syscall(5, 3, 0, 0);

    //dev_write(tty0_devno, &test, sizeof(test)-1);
    syscall(DEV_WRITE, tty0_devno, (uint32_t) &shell_name, sizeof(shell_name) - 1);
    
    while(1) {
        char buffer[16] = {0};
        //write prompt
        syscall(DEV_WRITE, tty0_devno, (uint32_t) &prompt, sizeof(prompt) - 1);
        //read input
        int size = syscall(DEV_READ, tty0_devno, (uint32_t) &buffer, sizeof(buffer));

        if (strncmp(buffer, "uname", size) == 0) {
            syscall(DEV_WRITE, tty0_devno, (uint32_t) &uname, sizeof(uname) - 1);
        } else if (strncmp(buffer, "exit", size) == 0) {
            break;
        } else if (strncmp(buffer, "clear", size) == 0) {
            syscall(DEV_IOCTL, tty0_devno, TTY_IOCTL_CLEAR, 0);
        } else if (strncmp(buffer, "tilegpu", size) == 0) {
            syscall(DEV_IOCTL, gpu0_devno, TILEGPU_IOCTL_DRAWTILE, (uint32_t) &(struct tilegpu_ioctl_msg_drawtile){'a', 0, 0, TILEGPU_DRAW});
            syscall(DEV_IOCTL, gpu0_devno, TILEGPU_IOCTL_DRAWTILE, (uint32_t) &(struct tilegpu_ioctl_msg_drawtile){'b', 2, 2, TILEGPU_DRAW});
        } else {
            syscall(DEV_WRITE, tty0_devno, (uint32_t) &bad_command, sizeof(bad_command) - 1);
        }
    }

    //exit(0)
    syscall(EXIT, 0, 0, 0);
}

void main() {
    *((uint8_t*) 0xFFF6) = 0;
    
    //initialize functions
    device_init();
    tty_init();
    tilegpu_init();
    gen_disk_init();
    proc_init();

    tty0 = device_create(&tty0_devno, TTY_MAJOR, (void*) 0xFFF1);
    gpu0 = device_create(&gpu0_devno, TILEGPU_MAJOR, (void *) 0xFFF9);

    proc_create((uint32_t) &init_thread, (uint32_t) &init_thread_stack + 64);
    proc_create((uint32_t) &test_thread, (uint32_t) &test_thread_stack + 128);

    //bootstrap the scheduler by getting the first process to run
    current_process = proc_dequeue();
    exit_kernel(); //jump into the current process
}


