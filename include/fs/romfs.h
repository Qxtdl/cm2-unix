#pragma once
#include <fs/fs.h>

struct romfs_inode {
    struct inode base;
    void* data;
    uint32_t length;
};

#define ROMFS_MAX_FILES 16

struct romfs_superblock {
    struct superblock base;
    struct romfs_inode file_table[ROMFS_MAX_FILES];
};

extern const struct super_ops romfs_sops;
extern const struct file_ops romfs_fops;

int8_t romfs_lookup(fs_lookup_t* state);
struct superblock* romfs_mount(struct device* dev, const char* args);

int8_t romfs_read(fs_read_t* state);


