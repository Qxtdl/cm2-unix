#include <fs/romfs.h>
#include <lib/stdlib.h>
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

struct romfs_file root_dir[2] = {
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
    }
};

static struct romfs_superblock romfs = {
    .base.sops = (struct super_ops*) &romfs_sops,
    .base.fops = (struct file_ops*) &romfs_fops,
    .root = &(struct romfs_file) {
        .length = 2,
        .data = root_dir
    }
};



int8_t romfs_lookup(fs_lookup_t* state)
{
    uint16_t size;
    struct romfs_file* dir;

    if (state->dir == NULL) {
        dir = romfs.root->data;
        size = romfs.root->length;
    } else {
        if (state->dir->mode == FS_MODE_FILE) {
            return -1; //this is not a dir
        }
        dir = state->dir->romfs.data;
        size = state->dir->romfs.length;
    }
    //empty name means return the root
    if (*state->fname == '\0') {
        struct inode* new = create_inode("");
        new->dir = 0;
        new->file = (uint32_t) dir;
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
            new->dir = (uint32_t) dir;
            new->file = (uint32_t) curr->data;
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

struct superblock* romfs_mount(struct device* dev, const char* args)
{
    return &romfs.base;
}

//fuck gcc
__attribute__((optimize("O0"))) int8_t romfs_read(fs_read_t* state)
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

__attribute__((optimize("O0"))) int8_t romfs_readdir(fs_read_t* state)
{
    struct inode* dir = state->descriptor->file;
    if (dir->mode == FS_MODE_FILE) {
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
    dirent->d_ino = (uint32_t) file_entry->data;
    strncpy(dirent->name, (char*) file_entry->name, FS_INAME_LEN);
    
    if (i == state->count) {
        return 1;
    }
    if (i == dir->romfs.length) {
        return 1;
    }
    return 0;
}



