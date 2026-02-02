#pragma once

#include <stdint.h>
#include <kernel/device.h>

#define TILEGPU_X           ((volatile uint8_t  *)0xFFFF)
#define TILEGPU_Y           ((volatile uint8_t  *)0xFFFE)
#define TILEGPU_ADDR        ((volatile uint16_t *)0xFFFC)
#define TILEGPU_FX_OPCODE   ((volatile uint8_t  *)0xFFFB)
#define TILEGPU_FX_IMM      ((volatile uint16_t *)0xFFFA)
#define TILEGPU_DRAW        1
#define TILEGPU_CLEAR       2
#define TILEGPU_CONTROLS    ((volatile uint8_t  *)0xFFF9)


#define TILEGPU_NO_FX       0
#define TILEGPU_AND_COLOR   1
#define TILEGPU_NAND_COLOR  2
#define TILEGPU_XOR_COLOR   3
#define TILEGPU_OR_COLOR    4
#define TILEGPU_NOR_COLOR   5

//optional, this is just using a struct for the hardware registers instead of defines
struct tilegpu_hw_interface;

struct tilegpu_device {
    struct device base;//inherit from device
    volatile struct tilegpu_hw_interface* gpu; //pointer to where the gpu is mapped, also optional if you want to use defines or tilegpu_hw_interface
    struct device_request* current_req; //this is the current request the device is handeling
    uint32_t current_pixels_copied; //this is the amount of bytes that were copied to or from the user buffer
};

#define TILEGPU_IOCTL_CLEAR 0
#define TILEGPU_IOCTL_DRAWTILE 1

struct tilegpu_ioctl_msg_drawtile {
    uint16_t tile_id;
    uint8_t x;
    uint8_t y;
    uint8_t controls;
};

void tilegpu_init();

struct device* tilegpu_create(int8_t* minor, const void* args); //create an instance, we only have one gpu so this should be simple
int tilegpu_destroy(uint8_t minor); //destroy an instance, doesnt do anything except maybe uninitialize the gpu
struct device* tilegpu_lookup(uint8_t minor); //lookup a gpu instance, once again we only have one gpu so we only accept a minor of 0

int tilegpu_ioctl(struct device* dev, int cmd, void* arg); //send a command to the gpu, e.g draw or fxdraw 
void tilegpu_update(struct device* dev); //this gets called every tick, look at the current request, check if its a read or a write request, do a single operation e.g write a single character and do the bytes_copied++, this way you can write to the gpu without freezing the system
void tilegpu_global_update(); //this calls the update of all registered devices, since there is only one you can directly call tilegpu_update
