#include <fs/vfs.h>
#include <stddef.h>
#include <lib/stdlib.h>

struct superblock* rootfs;

void walk_path_init(path_walk_t* state, const char* path)
{
    memset(state->path_cpy, 0, FS_PATH_LEN);
    if (*path == '/') { //absolute path
        path++;
    } else {
        //relative paths not suported yet
    }
    strncpy(state->path_cpy, (char*) path, FS_PATH_LEN);
    state->fs_state.fname = state->path_cpy;
    state->path_ptr = state->path_cpy;
    state->fs_state.dir = NULL;
    state->fs_state.fs = rootfs; //we start the search at the root
}


int8_t walk_path(path_walk_t* state)
{
    if (*state->path_ptr == '\0') {
        int8_t stat = lookup_dir(&state->fs_state);
        if (stat != 0) {
            return stat;
        }
    }
    
    int8_t stat = 1;
    
    if (*state->path_ptr == '/') {
        *state->path_ptr = '\0';
        stat = lookup_dir(&state->fs_state);
        if (stat < 0) {
            return stat; //directory not found
        } else if (stat == 1) {
            state->fs_state.fname = state->path_ptr + 1;
        }
    }
    
    if (stat == 1) {
        state->path_ptr++;
    }

    return 0; //continue
}


int mount_root(const char* fs_name, dev_t devno)
{
    struct super_ops* fs = lookup_filesystem(fs_name);
    if (fs == NULL) {
        return -1;
    }
    struct device* dev = device_lookup(devno);
    
    rootfs = fs->mount(dev, NULL);
    return 0;
}

int8_t mount_init(vfs_mount_t* state, const char* path, const char* fs_name, dev_t devno)
{
    state->fs = lookup_filesystem(fs_name);
    if (state->fs == NULL) {
        return -1;
    }
    state->dev = device_lookup(devno);
    if (state->dev == NULL) {
        return -1;
    }
    walk_path_init(&state->path, path);
    return 0;
}

int8_t mount_update(vfs_mount_t* state)
{
    int8_t stat = walk_path(&state->path);
    if (stat < 0) {
        return stat;
    }
    if (stat == 1) {
        struct inode* i = state->path.fs_state.dir;
        i->fs = state->fs->mount(state->dev, NULL);
        i->mode = FS_MODE_MOUNT;
        return 1;
    }
    return 0;
}


void vfs_close(struct fd* descriptor)
{
    struct inode* i = descriptor->file;
    if (i == NULL || i->refcount == 0) {
        return;
    }

    if (--i->refcount == 0) {
        free_inode(i);
    }

    descriptor->file = NULL;
}



