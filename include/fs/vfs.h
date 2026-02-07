#pragma once
#include <fs/fs.h>
#include <kernel/device.h>

typedef struct {
    char path_cpy[FS_PATH_LEN];
    char* path_ptr;
    fs_lookup_t fs_state;
} path_walk_t;

typedef struct {
    path_walk_t path;
    struct super_ops* fs;
    struct device* dev;
} vfs_mount_t;

void walk_path_init(path_walk_t* state, const char* path);
int8_t walk_path(path_walk_t* state);

int mount_root(const char* fs_name, dev_t devno);
int8_t mount_init(vfs_mount_t* state, const char* path, const char* fs_name, dev_t devno);
int8_t mount_update(vfs_mount_t* state);
void vfs_close(struct fd* decsriptor);


