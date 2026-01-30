#include <kernel/device.h>
#include <kernel/tty.h>
#include <kernel/majors.h>

const char init_msg[] = "CM2-UNIX V0.0.1-PRE\n";

void main(void) {
    tty_init();
    
    dev_t tty0_devno;
    struct device* tty0 = device_create(&tty0_devno, TTY_MAJOR, (void*) 0xFFF1); //0xFFF1 is the base address of tty0
    
    struct device_request rq0 = {
        .buffer = (char*) init_msg,
        .count = sizeof(init_msg),
        .offset = 0,
        .operation = DEVICE_OP_WR
    };
    device_queue_action(tty0, &rq0);

    while (1) {
        tty0->ops->update(tty0);
    }
}



