#include <stddef.h>
#include <lib/stdlib.h>
#include <fs/dcache.h>

struct dentry dentry_pool[DENTRY_MAX];
uint8_t dentry_free_list[DENTRY_MAX];
uint8_t dentry_free_list_top;

void dentry_release(uint8_t node) { //destroy a dentry
    struct dentry* dentry = &dentry_pool[node];

    struct inode* dir = dentry->dir;
    dir->fs->sops->release(dir->fs, dir);
    dentry->dir = NULL;
    dentry->fs = NULL;

    dentry_free_list[dentry_free_list_top++] = node;
}

//go through the children of the node, clean them and then remove each child that got evicted
void dentry_clean_node(uint8_t node)
{
    struct dentry* parent = &dentry_pool[node];
    uint8_t i = 0;
    uint8_t parent_child_count = parent->packed & DENTRY_CHILD_COUNT_MSK;

    while (i < parent_child_count) {
        uint8_t child_index = parent->children[i];
        struct dentry* child = &dentry_pool[child_index];

        dentry_clean_node(child_index);
        uint8_t child_count = child->packed & DENTRY_CHILD_COUNT_MSK;
        uint8_t locked = child->packed & DENTRY_LOCKED_MSK;

        if ((child_count == 0) && (!locked)) { //the child got evicted
            uint8_t last = parent_child_count - 1; //remove the index by popping a value of  the children stack and writing it over the old value
            parent->children[i] = parent->children[last];
            parent->children[last] = DENTRY_INDEX_NIL;
            parent_child_count = last;
        } else {
            i++;
        }
    }

    if ((parent_child_count == 0) && !(parent->packed & DENTRY_LOCKED_MSK)) {
        dentry_release(node);
    }
    parent->packed = (parent->packed & ~DENTRY_CHILD_COUNT_MSK) | parent_child_count;
}

uint8_t dentry_alloc()
{
    uint8_t tmp = dentry_free_list_top;
    if (tmp == 0) {
        dentry_clean_node(DENTRY_ROOT);
    }
    
    tmp = dentry_free_list_top;
    if (tmp == 0) {
        return DENTRY_INDEX_NIL;
    }
    uint8_t new = dentry_free_list[--tmp];
    dentry_free_list_top = tmp;
    return new;
}

int dentry_add_child(struct dentry* dentry, uint8_t child) {
    uint8_t index = dentry->packed & DENTRY_CHILD_COUNT_MSK;
    if (index <= DENTRY_MAX_CHILDREN) {
        dentry->children[index++] = child;
        dentry->packed = (dentry->packed & ~DENTRY_CHILD_COUNT_MSK) | index;
        return 0;
    }
    return 1;
}

uint8_t dentry_lookup(const char* path)
{ 
    char path_cpy[FS_PATH_LEN] = {0};
    strncpy(path_cpy, (char*) path, FS_PATH_LEN);
    char* path_ptr = path_cpy;
    char* current_word = path_cpy;
    
    uint8_t current_dentry = 0;

    //parse the path and lookup each dir from within the previous dir
    while(*path_ptr != '\0') {
        if (*path_ptr == '/') {
            *path_ptr = '\0';
            current_dentry = dentry_local_lookup(current_dentry, current_word);
            if (current_dentry == 0) { //0 is reserved for the root, so it can never be returned
                return DENTRY_INDEX_NIL;
            }
            current_word = path_ptr + 1;
        }
        path_ptr++;
    }

    current_dentry = dentry_local_lookup(current_dentry, current_word);
    if (current_dentry == 0) {
        return DENTRY_INDEX_NIL;
    }
    return current_dentry;
}

uint8_t dentry_local_lookup(uint8_t dentry_index, char* name)
{
    struct dentry* parent = &dentry_pool[dentry_index];
    uint8_t child_count = parent->packed & DENTRY_CHILD_COUNT_MSK;
    for (uint8_t i = 0; i < child_count; i++) {
        uint8_t child_index = parent->children[i];
        struct dentry* child = &dentry_pool[child_index];

        if (strncmp(child->dir->name, name, FS_INAME_LEN) == 0) {
            return child_index;
        }
    }

    //dentry doesnt exist so we need to create one
    //if the parent is a mountpoint we need to pass NULL as the root so the fs nows were in the root
    struct inode* dir = parent->dir;
    if (parent->packed & DENTRY_MOUNT_POINT_MSK) {
        dir = NULL;
    }

    struct superblock* fs = parent->fs;
    struct inode* new_inode = fs->sops->lookup(fs, dir, name);
    if (new_inode == NULL) {
        return DENTRY_INDEX_NIL;
    }
    parent->packed |= DENTRY_LOCKED_MSK; //set the locked flag, this is so that the parent doesnt get evicted
    uint8_t new_index = dentry_alloc();
    parent->packed &= ~DENTRY_LOCKED_MSK; //reset the locked flag
    if (new_index == DENTRY_INDEX_NIL) {
        return DENTRY_INDEX_NIL;
    }

    struct dentry* new_child = &dentry_pool[new_index];
    new_child->packed = 0;
    new_child->dir = new_inode;
    new_child->fs = fs;
    
    if (dentry_add_child(parent, new_index)) {
        return DENTRY_INDEX_NIL;
    }

    return new_index;

}

int dentry_mount(struct superblock* fs, const char* mountpoint)
{
    uint8_t mount_index = dentry_lookup(mountpoint);
    struct dentry* mount = &dentry_pool[mount_index];
    if (mount_index == DENTRY_INDEX_NIL) {
        return -1;
    }
    
    dentry_clean_node(mount_index); //remove any cached nodes from the mountpoint
    uint8_t packed = mount->packed;
    
    if (((packed & DENTRY_CHILD_COUNT_MSK) != 0) || (packed & DENTRY_MOUNT_POINT_MSK)) { //we cant overwrite another mountpoint since that would be bad
        return -1;
    }

    mount->fs = fs;

    return 0; //success!
}

int dentry_umount(const char* mountpoint) {
    uint8_t mount_index = dentry_lookup(mountpoint);
    struct dentry* mount = &dentry_pool[mount_index];

    if (mount_index == DENTRY_INDEX_NIL) {
        return -1;
    }
    //implement unmounting logic
    return 0;
}



