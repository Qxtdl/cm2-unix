#pragma once

#include <kernel/device.h>
#include <stdint.h>

struct superblock;
struct fd;
struct dentry;

typedef uint32_t ino_t; //this is a unique identifier for an inode, this would be like the cluster + offset in fat

#define FS_INAME_LEN 10
#define FS_PATH_LEN (FS_INAME_LEN*4)
#define FS_MODE_DIR 1
#define FS_MODE_FILE 2
#define FS_MODE_MOUNT 3

#define INODE_TABLE_SIZE 32

struct romfs_inode {
    void* data;
    uint32_t length;
};


struct inode {
    struct superblock* fs; //this is the filesystem that the inode is part of
    char name[FS_INAME_LEN]; //the name of the file
    uint8_t refcount; //the amount of references exist to this inode, this includes file descriptors and dentries
    uint8_t mode; //currently only stores filetype, but i will later expand it will actual permissions and ownership
    ino_t dir; //the parent directory of this inode
    ino_t file; //the inum of this inode
    union {
        struct romfs_inode romfs;
    };
};

struct stat {
    uint32_t size;
    uint32_t time;
    uint8_t mode;
    dev_t dev;
    ino_t d_ino;
    char name[FS_INAME_LEN];
};

typedef struct {
    struct device_request* req;
    struct superblock* fs;
    struct inode* dir;
    char* fname;
} fs_lookup_t;

typedef struct {
    struct device_driver* req;
    struct superblock* fs;
    struct fd* descriptor;
    uint32_t bytes_read;
    uint32_t count;
    void* buffer;
} fs_read_t;

typedef struct {
    struct device_driver* req;
    struct superblock* fs;
    struct fd* descriptor;
    uint32_t bytes_written;
    uint32_t count;
    void* buffer;
} fs_write_t;

typedef struct {
    struct device_driver* req;
    struct superblock* fs;
    struct fd* descriptor;
    uint32_t bytes_read;
    uint32_t count;
    void* buffer;
    uint32_t offset;
} fs_pread_t;

typedef struct {
    struct device_driver* req;
    struct superblock* fs;
    struct fd* descriptor;
    uint32_t bytes_written;
    uint32_t count;
    void* buffer;
    uint32_t offset;
} fs_pwrite_t;


typedef struct {
    struct device_request* req;
    struct inode* dir;
    char* name;
} fs_dirop_t;

//TODO: the current api is not very good but i can chage it later
//these operate on file systems
struct super_ops {
    int8_t (*lookup)(fs_lookup_t* state); //lookup an inode in a dir
    struct superblock* (*mount)(struct device* dev, const char* args);
    int (*umount)(struct superblock* fs);
    int (*mkdir)(fs_dirop_t* state);
    int (*rmdir)(fs_dirop_t* state);
    int (*unlink)(fs_dirop_t* state);
};

//these operate on file descriptors
struct file_ops {
    int8_t (*read)(fs_read_t* state);
    int8_t (*write)(fs_read_t* state);
    int (*lstat)(struct superblock* fs, struct fd* f, struct stat* statbuff); //these are instant (i hope)
    int (*lseek)(struct superblock* fs, struct fd* f, uint32_t offset, int whence);
    void (*close)(struct superblock* fs, struct fd* f);
    int8_t (*readdir)(fs_read_t* state);
    int8_t (*pread)(fs_pread_t* state);
    int8_t (*pwrite)(fs_pwrite_t* state);
};

struct superblock {
    struct super_ops* sops;
    struct file_ops* fops;
};

//file flag defenitions
#define FD_R (1 << 0)
#define FD_W (1 << 1)
#define FD_CREAT (1 << 2)
#define FD_NONBLOCK (1 << 3)

struct fd {
    struct inode* file;
    uint32_t offset : 24;
    uint8_t flags : 8;
};

#define MAX_FILESYSTEM_COUNT 4
#define FS_NAME_LEN 4
#define MAX_FD 32
extern struct fd fd_table[MAX_FD];


void fs_init();
struct inode* create_inode(const char* name);
void free_inode(struct inode* i);
int8_t lookup_dir(fs_lookup_t* state);
void register_filesystem(const char* name, struct super_ops* fs);
struct super_ops* lookup_filesystem(const char* name);
int fd_alloc();



