#include <fs/romfs.h>
#include <lib/stdlib.h>


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
            .base = {
                .name = "yeet"
            },
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
        struct romfs_inode* curr = &romfs.file_table[i];

        if (strncmp(curr->base.name, state->fname, FS_INAME_LEN) == 0) {
            state->dir = &curr->base;
            return 1;
        }
    }

    return -1;
}

struct superblock* romfs_mount(struct device* dev, const char* args)
{
    return &romfs.base;
}


int8_t romfs_read(fs_read_t* state)
{
    struct romfs_inode* file = (struct romfs_inode*) state->descriptor->file;
    uint32_t i = state->bytes_read;

    uint8_t byte = ((uint8_t*)file->data)[i];

    ((uint8_t*) state->buffer)[i] = byte;
    state->bytes_read = ++i;

    if (i == state->count || i == file->length) {
        return 1;
    }
    return 0;
}


