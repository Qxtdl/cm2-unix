#include <fs/romfs.h>
#include <lib/stdlib.h>
#include <kernel/panic.h>

const struct super_ops romfs_sops = {
    .lookup = &romfs_lookup,
    .mount = &romfs_mount
};

const struct file_ops romfs_fops = {
    .read = &romfs_read
};

static struct romfs_superblock romfs = {
    .base.sops = (struct super_ops*) &romfs_sops,
    .base.fops = (struct file_ops*) &romfs_fops,
    .file_table = {
        {
            .name = "yeet.txt",
            .mode = FS_MODE_FILE,
            .data = "Hello world!",
            .length = 12
        }
    }
};



int8_t romfs_lookup(fs_lookup_t* state)
{
    if (state->dir != NULL) {
        return -1; //romfs does not suport directories (yet)
    }

    for (int i = 0; i < ROMFS_MAX_FILES; i++) {
        struct romfs_file* curr = &romfs.file_table[i];

        if (strncmp(curr->name, state->fname, FS_INAME_LEN) == 0) {
            struct inode* new = create_inode();
            new->dir = 0;
            new->file = (uint32_t) curr->data;
            new->fs = &romfs.base;
            new->mode = curr->mode;
            strncpy(new->name, (char*) curr->name, FS_INAME_LEN);
            new->refcount = 0;
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


int8_t romfs_read(fs_read_t* state)
{
    
    struct inode* file = state->descriptor->file;
    uint32_t i = state->bytes_read;

    uint8_t byte = ((uint8_t*)file->romfs.data)[i];
    debug('F');
    ((uint8_t*) state->buffer)[i] = byte;
    state->bytes_read = ++i;


    if (i == state->count || i == file->romfs.length) {
        debug('Y');
        return 1;
    }
    return 0;
}


