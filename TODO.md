
# TODO's

## General
* seperate architecture dependent code better.
* add configuration ability to makefile using either a .conf file or text user interface.
* make buffer and queue sizes configurable via makefile.
* make kernel panic more descriptive, i.e show more registers and status.
* add error handeling to most of the kernel code like returning proper error codes and calling panic() in unrecoverable errors.

## VFS

* find out how to bind syscalls to VFS calls
* implement filesystem e.g FAT
* implement devfs
* maybe add pipefs?
    - maybe bidirectional?
    - maybe not make it part of the VFS?


## Syscalls

* add posix_spawn like syscall
* add vfs syscalls
* add pipe syscalls

