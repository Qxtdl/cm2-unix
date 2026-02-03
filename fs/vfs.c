#include <fs/vfs.h>
#include <stddef.h>
#include <lib/stdlib.h>

//this is just a private type
#define FS_NAME_LEN 4
struct fs_tuple {
    struct super_ops* fs;
    char name[FS_NAME_LEN];
};

//this maps names to fileystem types
#define MAX_FILESYSTEM_COUNT 4
struct fs_tuple filesystem_registry[MAX_FILESYSTEM_COUNT];

#define MAX_FD 32
struct fd fd_table[MAX_FD];

void register_filesystem(const char* name, struct super_ops* fs)
{
    for (int i = 0; i < MAX_FILESYSTEM_COUNT; i++) {
        if (filesystem_registry[i].fs == NULL) {
            filesystem_registry[i].fs = fs;
            strncpy(filesystem_registry[i].name, (char*) name, FS_NAME_LEN);
            break;
        }
    }
}


//maybe there is a faster way of allocating these?
int fd_alloc()
{
    for (int i = 0; i < MAX_FD; i++) {
        if (fd_table[i].file == NULL) {
            return i;
        }
    }
    return -1;
}

void walk_path_init(path_walk_t* state, const char* path)
{
    strncpy(state->path_cpy, (char*) path, FS_PATH_LEN);
    state->fs_state.fname = state->path_cpy;
    state->path_ptr = state->path_cpy;
    state->fs_state.dir = NULL;
}

uint8_t walk_path(path_walk_t* state)
{
    if (*state->path_ptr == '\0') {
        uint8_t stat = state->fs_state.fs->sops->lookup(&state->fs_state);
        if (stat != 0) {
            return stat;
        }
    }
    
    uint8_t stat = 1;
    
    if (*state->path_ptr == '/') {
        *state->path_ptr = '\0';
        stat = state->fs_state.fs->sops->lookup(&state->fs_state);
        
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






