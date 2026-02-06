#include <fs/vfs.h>
#include <stddef.h>
#include <lib/stdlib.h>

struct superblock* rootfs;

void walk_path_init(path_walk_t* state, const char* path)
{
    memset(state->path_cpy, 0, FS_PATH_LEN);
    strncpy(state->path_cpy, (char*) path, FS_PATH_LEN);
    state->fs_state.fname = state->path_cpy;
    state->path_ptr = state->path_cpy;
    state->fs_state.dir = NULL;
    state->fs_state.fs = rootfs; //we start th search at the root
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


