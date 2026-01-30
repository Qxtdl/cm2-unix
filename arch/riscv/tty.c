#include <stddef.h>
#include <kernel/device.h>
#include <kernel/majors.h>
#include <kernel/tty.h>


//the hardware interface implementation
struct tty_hardware_interface {
    char input_character;
    uint8_t input_ready;
    uint8_t clear;
    uint8_t write;
    char character;
    uint8_t cursor_location;
};

//this is the table that stores the tty instances
#define MAX_TTY_COUNT 4
#define MAX_TTY_MSK (MAX_TTY_COUNT-1)
struct tty_device tty_table[MAX_TTY_COUNT];

//tty operations
const struct device_ops tty_ops = {
    .read = &tty_read,
    .write = &tty_write,
    .ioctl = &tty_ioctl
};

void tty_init() {
    struct device_driver* tty_driver = &driver_registry[TTY_MAJOR];
    tty_driver->create = &tty_create;
    tty_driver->destroy = &tty_destroy;
    tty_driver->lookup = &tty_lookup;
    tty_driver->name = "generic tty";
}

//create a tty instance
struct device* tty_create(int8_t* minor, const void* args)
{
    for (int8_t i = 0; i < MAX_TTY_COUNT; i++) {
        struct tty_device* tty = &tty_table[i];
        if (tty->base.ops == NULL) {
            tty->base.ops = &tty_ops;
            tty->tty = (struct tty_hardware_interface*) args;
            *minor = i;
            return &tty->base;
        }
    }
    *minor = -1; //TODO: add error code for this
    return NULL;
}

//destroy a tty instance
int tty_destroy(uint8_t minor)
{
    struct tty_device* tty = &tty_table[minor & MAX_TTY_MSK];
    tty->base.ops = NULL;
    tty->tty = NULL;
    return 0;
}

//lookup a tty instance
struct device* tty_lookup(uint8_t minor)
{
    struct tty_device* tty = &tty_table[minor & MAX_TTY_MSK];
    if (tty->base.ops == NULL) {
        return NULL;
    }
    return &tty->base;
}

//read bytes from the tty
uint32_t tty_read(struct device* dev, void* buffer, uint32_t count, uint32_t offset)
{
    //TODO: implement reading

    return 0;
}

//write bytes to the tty
uint32_t tty_write(struct device* dev, const void* buffer, uint32_t count, uint32_t offset)
{
    struct tty_device* tty = (struct tty_device*) dev;
    volatile struct tty_hardware_interface* tty_interface = tty->tty;
    if (tty->base.ops == NULL) {
        return 0;
    }

    //NOTE: i would have wished for a scroll buffer but thats just way too much of a waste of space and cpu time
    const char* str = buffer;
    for (uint32_t i = 0; i < count; i++) {

        if (str[i] == '\n' || tty_interface->cursor_location == 255)
        {
            tty_interface->cursor_location = ((tty_interface->cursor_location + 32) & 0b11100000);
            continue;
        }
        tty_interface->character = str[i];
        tty_interface->cursor_location++;
        tty_interface->write = 1;
    }
    
    return count;
}

int tty_ioctl(struct device* dev, int cmd, void* arg) {
    struct tty_device* tty = (struct tty_device*) dev;
    struct tty_hardware_interface* tty_interface = tty->tty;
    int return_code = 0;

    if (tty->base.ops == NULL) {
        return 0;
    }


    //TODO: implement more ioctl's, create documentation and user facing header, create error code
    if (cmd == TTY_IOCTL_CLEAR) {
        tty_interface->clear = 1;
        tty_interface->cursor_location = 0;
    } else if (cmd == TTY_IOCTL_SETCURSOR) {
        int* location = arg;
        tty_interface->cursor_location = *location;
    } else {
        return_code = -1;
    }
    return return_code;
}



