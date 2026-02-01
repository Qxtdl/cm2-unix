
# TODO's

## General
* seperate architecture dependent code better
* add configuration ability to makefile using either a .conf file or text user interface
* make buffer and queue sizes configurable via makefile

## VFS

* find out how to bind syscalls to VFS calls
* implement filesystem e.g FAT
* implement devfs
* maybe add pipefs?
    - maybe bidirectional?
    - maybe not make it part of the VFS?


## Syscalls

* add posix_spawn like syscall
* add pidwait syscall
* implement exit syscall with return code
* add vfs syscalls
* add pipe syscalls

