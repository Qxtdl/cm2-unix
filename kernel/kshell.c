#include <kernel/kshell.h>

#include <lib/stdlib.h>
#include <lib/kprint.h>

#include <kernel/device.h>
#include <kernel/majors.h>
#include <kernel/syscall.h>

#include <kernel/block.h>
#include <kernel/tilegpu.h>
#include <kernel/tty.h>

#include <fs/fs.h>
#include <fs/vfs.h>
#include <fs/devfs.h>
#include <fs/romfs.h>



[[gnu::aligned(16)]] uint8_t kshell_thread_stack[KSHELL_STACK_SIZE];
const char shell_name[] = "Shell v0.2.0\n";
const char prompt[] = "# ";
const char uname[] = "CM2-UNIX V0.3.2\nbuild: " __DATE__ "\n";
const char bad_command[] = "-shell: bad cmd\n";
const char ls_ftypes[] = "dmfn";

#define LINE_SIZE 16

const char* argv[4];

int parse_args(char* buffer, int size)
{
    int argc = 0;
    char* current_arg = buffer;
    for (int i = 0; i < size; i++) {
        if (buffer[i] == ' ') {
            buffer[i] = '\0';
            argv[argc++] = current_arg;
            current_arg = &buffer[i + 1];
        }
    }
    argv[argc++] = current_arg;

    return argc;
}


void kshell_thread()
{
    int tty0 = syscall(OPEN, (uint32_t) "/dev/tty0", 0, 0);
    char input[LINE_SIZE];

    while(1) {
        memset(input, 0, LINE_SIZE);
        syscall(WRITE, tty0, (uint32_t) prompt, 2);

        int size = syscall(READ, tty0, (uint32_t) input, LINE_SIZE);
        //BUG: the kernel seems to hang in the proc_update function sometimes due to this read syscall
        int argc = parse_args(input, size);
        if (argc == 0) {
            continue;
        }
        
        if (strncmp(argv[0], "uname", LINE_SIZE) == 0) {
            syscall(WRITE, tty0, (uint32_t) uname, sizeof(uname) - 1);
        
        } else if (strncmp(argv[0], "clear", LINE_SIZE) == 0) {
            syscall(IOCTL, tty0, TTY_IOCTL_CLEAR, 0);

        } else {
            syscall(WRITE, tty0, (uint32_t) bad_command, sizeof(bad_command) - 1);
        }

    }


    syscall(EXIT, 0, 0, 0);
}


