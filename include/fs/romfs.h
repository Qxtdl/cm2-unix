#pragma once
#include <fs/fs.h>


#define ROMFS_DIR_SIZE 16

struct romfs_file {
    const char* name;
    uint16_t length;
    uint8_t mode;
    void* data;
};

struct romfs_superblock {
    struct superblock base;
    struct romfs_file* root;
};

extern const struct super_ops romfs_sops;
extern const struct file_ops romfs_fops;

int8_t romfs_lookup(fs_lookup_t* state);
struct superblock* romfs_mount(struct device* dev, const char* args);

int8_t romfs_read(fs_read_t* state);
int8_t romfs_readdir(fs_read_t* state);


