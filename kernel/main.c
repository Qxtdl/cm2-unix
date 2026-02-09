#include <lib/kprint.h>
#include <lib/stdlib.h>

#include <kernel/device.h>
#include <kernel/tty.h>
#include <kernel/block.h>
#include <kernel/tilegpu.h>
#include <kernel/majors.h>
#include <kernel/proc.h>
#include <kernel/syscall.h>
#include <kernel/panic.h>

#include <fs/romfs.h>
#include <fs/devfs.h>

#include <kernel/kshell.h>

void main() {
    dev_t tty0_devno, gpu0_devno, disk0_devno;

    //initialize functions
    device_init();
    tty_init();
    tilegpu_init();
    gen_disk_init();
    proc_init();
    fs_init();

    kputs("CM2-UNIX v0.3.4\nBooting...\n");
    
    device_create(&tty0_devno, TTY_MAJOR, (void*) 0xFFF1);
    device_create(&gpu0_devno, TILEGPU_MAJOR, &(struct tilegpu_hw_interface){
        .controls = TILEGPU_CONTROLS,
        .fx_imm = TILEGPU_FX_IMM,
        .fx_opcode = TILEGPU_FX_OPCODE,
        .tile_id = TILEGPU_ADDR,
        .y = TILEGPU_Y,
        .x = TILEGPU_X
    });
    device_create(&disk0_devno, GEN_DISK_MAJOR, (void*) 0xFFC3);
    kputs("Registered devices\n");

    register_filesystem("romfs", (struct super_ops*) &romfs_sops);
    register_filesystem("devfs", (struct super_ops*) &devfs_sops);

    mount_root("romfs", 255);
    if (mount_devfs("devfs") < 0) {
        panic("devfs mount failed");
    }
    kputs("mounted rootfs and devfs\n");

    devfs_create_handle("tty0", tty0_devno);
    devfs_create_handle("gpu0", gpu0_devno);
    devfs_create_handle("disk0", disk0_devno);
    kputs("populated devfs\n");

    proc_create((uint32_t) &kshell_thread, (uint32_t) &kshell_thread_stack + KSHELL_STACK_SIZE);

    //bootstrap the scheduler by getting the first process to run
    current_process = proc_dequeue();
    exit_kernel(); //jump into the current process
}


