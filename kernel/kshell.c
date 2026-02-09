#include <kernel/kshell.h>


#include <lib/stdlib.h>
#include <lib/kprint.h>
#include <lib/hex.h>

#include <uapi/dev.h>
#include <uapi/majors.h>
#include <uapi/syscalls.h>

#include <uapi/tty.h>
#include <uapi/tilegpu.h>

#include <uapi/vfs.h>

[[gnu::aligned(16)]] uint8_t kshell_thread_stack[KSHELL_STACK_SIZE];
const char shell_name[] = "Shell v0.2.1\n";
const char prompt[] = "# ";
const char* errno_types[6] = {
    ": command not found.\n",
    ": invalid number of args.\n",
};

const char ls_ftypes[] = "dmfn";
int tty0;

#define LINE_SIZE 24

char* argv[4];

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

int open(const char* path, uint32_t flags)
{
    return syscall(SYS_OPEN, (uint32_t) path, flags, 0);
}

int read(int fd, void* buffer, int count)
{
    return syscall(SYS_READ, fd, (uint32_t) buffer, count);
}

int write(int fd, const void* buffer, int count)
{
    return syscall(SYS_WRITE, fd, (uint32_t) buffer, count);
}

int readdir(int dirfd, struct stat* buffer, int count)
{
    return syscall(SYS_READDIR, dirfd, (uint32_t) buffer, count);
}

int ioctl(int fd, int cmd, void* arg)
{
    return syscall(SYS_IOCTL, fd, cmd, (uint32_t) arg);
}

void exit(int exit_code)
{
    syscall(SYS_EXIT, exit_code, 0, 0);
}

int sysctl(int cmd, void* buff, int count)
{
    return syscall(SYS_SYSCTL, cmd, (uint32_t) buff, count);
}

#define LS_MAX 8
struct stat ls_buff[LS_MAX];

void ls_cmd(char* path)
{

    int dirfd = open(path, 0);

    int count = readdir(dirfd, ls_buff, LS_MAX);
    
    for (int i = 0; i < count; i++) {
        char buff[] = "x           xxxx\n";
        struct stat* curr = &ls_buff[i];
        
        buff[0] = ls_ftypes[curr->mode];
        int_to_hex(buff + 12, curr->size, 4);
        strncpy(buff + 2, curr->name, FS_INAME_LEN);
        
        write(tty0, buff, sizeof(buff) - 1);
    }

}

char cat_buffer[32];

void cat_cmd(char* path)
{
    int fd = open(path, 0);
    int size = read(fd, cat_buffer, 32);
    if (size < 0) {
        const char* err = "cat: could not open file\n";
        write(tty0, err, strlen(err));
        return;
    }
    write(tty0, cat_buffer, size);
    write(tty0, "\n", 1);
}


void kshell_thread()
{
    tty0 = open("/dev/tty0", 0);
    char input[LINE_SIZE];
    
    while(1) {
        int errno = 0;
        memset(input, 0, LINE_SIZE);
        write(tty0, prompt, 2);

        int size = read(tty0, input, LINE_SIZE - 1);
        
        int argc = parse_args(input, size);
        if (argc == 0) {
            continue;
        }
        
        if (strncmp(argv[0], "ls", LINE_SIZE) == 0) {
            if (argc == 1) {
                ls_cmd("/");
            } else if (argc == 2) {
                ls_cmd(argv[1]);
            } else {
                errno = 2;
            }
        } else if (strncmp(argv[0], "cat", LINE_SIZE) == 0) {
            if (argc != 2) {
                errno = 2;
            } else {
                cat_cmd(argv[1]);
            }
        } else if (strncmp(argv[0], "uname", LINE_SIZE) == 0) {
            char ubuff[40];
            sysctl(1, ubuff, 40);
            write(tty0, ubuff, strlen(ubuff));
        
        } else if (strncmp(argv[0], "clear", LINE_SIZE) == 0) {
            ioctl(tty0, TTY_IOCTL_CLEAR, 0);

        } else if (strncmp(argv[0], "exit", LINE_SIZE) == 0) {
            break;

        } else {
            errno = 1;
        }
        
        if (errno > 0) {
            write(tty0, argv[0], strnlen(argv[0], 4));
            const char* err_str = errno_types[errno - 1];
            write(tty0, err_str, strnlen(err_str, 32));
        }
    }

    exit(0);
}


