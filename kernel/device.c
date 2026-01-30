#include <kernel/device.h>
#include <stddef.h>

struct device_driver driver_registry[DEVICE_DRIVER_MAX];


int device_queue_action(struct device* dev, struct device_request* req)
{
    if (dev->count == DEVICE_MAX_REQ) return -1;
    
    dev->request_queue[dev->tail] = req;
    dev->tail = (dev->tail + 1) & DEVICE_MAX_REQ_MSK;
    dev->count++;

    return 0;
}

struct device_request* device_queue_pop(struct device* dev)
{
    if (dev->count == 0) return NULL;
    
    struct device_request* req = dev->request_queue[dev->head];
    dev->head = (dev->head + 1) & DEVICE_MAX_REQ_MSK;
    dev->count--;
    
    return req;
}

struct device* device_create(dev_t* devno, uint8_t major, const void* args)
{
    struct device_driver* driver = &driver_registry[major];
    if (driver->create == NULL) {
        return NULL;
    }
    
    int8_t minor;
    struct device* device_instance = driver->create(&minor, args);
    
    if (minor < 0) {
        return NULL; //TODO: return the error code somehow
    }
    *devno = MKDEV(major, minor);
    return device_instance;
}

struct device* device_lookup(dev_t devno)
{
    uint8_t major = MAJOR(devno);
    uint8_t minor = MINOR(devno);
    struct device_driver* driver = &driver_registry[major];
    if (driver->lookup == NULL) {
        return NULL;
    }

    //TODO: we must return an error code somehow
    struct device* device_instance = driver->lookup(minor);
    return device_instance;
}

int device_destroy(dev_t devno)
{
    uint8_t major = MAJOR(devno);
    uint8_t minor = MINOR(devno);
    struct device_driver* driver = &driver_registry[major];
    if (driver->destroy == NULL) {
        return -1; //TODO: create error code for this
    }

    return driver->destroy(minor);
}


