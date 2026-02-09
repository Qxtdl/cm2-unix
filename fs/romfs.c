#include <fs/romfs.h>
#include <lib/stdlib.h>
#include <lib/kprint.h>
#include <kernel/panic.h>

const struct super_ops romfs_sops = {
    .lookup = &romfs_lookup,
    .mount = &romfs_mount
};

const struct file_ops romfs_fops = {
    .read = &romfs_read,
    .readdir = &romfs_readdir
};

struct romfs_file bin_dir[1] = {
    {
        .name = "test.txt",
        .length = 4,
        .mode = FS_MODE_FILE,
        .data = "yeet"
    }
};

struct romfs_file dev_dir[1];

struct romfs_file root_dir[3] = {
    {
        .name = "yeet.txt",
        .length = 12,
        .mode = FS_MODE_FILE,
        .data = "Hello world!"
    },
    {
        .name = "bin",
        .length = 1,
        .mode = FS_MODE_DIR,
        .data = bin_dir
    },
    {
        .name = "dev",
        .length = 0,
        .mode = FS_MODE_DIR,
        .data = dev_dir
    }
};

static struct romfs_superblock romfs = {
    .base.sops = (struct super_ops*) &romfs_sops,
    .base.fops = (struct file_ops*) &romfs_fops,
    .root = &(struct romfs_file) {
        .length = 3,
        .data = root_dir
    }
};



int8_t romfs_lookup(fs_lookup_t* state)
{
    uint16_t size;
    struct romfs_file* dir;

    if (state->dir->mode == FS_MODE_FILE) {
        return -1; //this is not a dir
    }

    dir = state->dir->romfs.data;
    size = state->dir->romfs.length;
    //empty name means return the root
    if (*state->fname == '\0') {
        struct inode* new = create_inode("");
        new->dir = state->dir->file;
        new->file = ROMFS_CREATE_INO(dir);
        new->fs = &romfs.base;
        new->mode = FS_MODE_DIR;
        new->romfs.length = size;
        new->romfs.data = dir;
        state->dir = new;
        return 1;
    }

    for (int i = 0; i < size; i++) {
        struct romfs_file* curr = &dir[i];

        if (strncmp(curr->name, state->fname, FS_INAME_LEN) == 0) {
            struct inode* new = create_inode(curr->name);
            new->dir = state->dir->file;
            new->file = ROMFS_CREATE_INO(curr->data);
            new->fs = &romfs.base;
            new->mode = curr->mode;
            new->romfs.length = curr->length;
            new->romfs.data = curr->data;
            state->dir = new;
            return 1;
        }
    }
    
    return -1; //file not found
}

int romfs_mount(struct inode* mountpoint, struct device* dev, const char* args)
{
    mountpoint->fs = &romfs.base;
    mountpoint->file = ROMFS_CREATE_INO(romfs.root->data);
    mountpoint->mode = FS_MODE_MOUNT;
    mountpoint->romfs.data = romfs.root->data;
    mountpoint->romfs.length = romfs.root->length;
    return 0;
}

int8_t romfs_read(fs_read_t* state)
{
    
    struct inode* file = state->descriptor->file;
    uint32_t i = state->bytes_read;

    uint8_t byte = ((uint8_t*)file->romfs.data)[i];
    ((uint8_t*) state->buffer)[i] = byte;
    state->bytes_read = ++i;


    if (i == state->count) {
        return 1;
    }

    if (i == file->romfs.length) {
        return 1;
    }
    return 0;
}

int8_t romfs_readdir(fs_read_t* state)
{
    struct inode* dir = state->descriptor->file;
    if (dir->mode != FS_MODE_DIR) {
        return -1;
    }

    uint32_t i = state->bytes_read;
    
    struct romfs_file* file_entry = &((struct romfs_file*) dir->romfs.data)[i];
    struct stat* dirent = &((struct stat*) state->buffer)[i];
    state->bytes_read = ++i;

    dirent->dev = 255;
    dirent->time = 0;
    dirent->mode = file_entry->mode;
    dirent->size = file_entry->length;
    dirent->d_ino = ROMFS_CREATE_INO(file_entry->data);
    strlcpy(dirent->name, (char*) file_entry->name, FS_INAME_LEN - 1);
    
    if (i == state->count) {
        return 1;
    }
    if (i == dir->romfs.length) {
        return 1;
    }
    return 0;
}



