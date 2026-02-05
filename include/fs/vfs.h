#pragma once
#include <fs/fs.h>
#include <kernel/device.h>

typedef struct {
    char path_cpy[FS_PATH_LEN];
    char* path_ptr;
    fs_lookup_t fs_state;
} path_walk_t;

void walk_path_init(path_walk_t* state, const char* path);
int8_t walk_path(path_walk_t* state);

int mount_root(const char* fs_name, dev_t devno);

int vfs_write(int fd, const void* buffer, uint32_t count);
int vfs_lstat(int fd, struct stat* statbuff);
int vfs_lseek(int fd, uint32_t offset, int whence);
void vfs_close(int fd);
int vfs_readdir(int fd, void* buffer, uint32_t count);
int vfs_pread(int fd, void* buffer, uint32_t count, uint32_t offset);
int vfs_pwrite(int fd, const void* buffer, uint32_t count, uint32_t offset);
void vfs_mount(const char* mount, const char* dev_path, const char* fs_name);
void vfs_umount(const char* mount);
int vfs_mkdir(const char* path);
int vfs_rmdir(const char* path);
int vfs_unlink(const char* path);
