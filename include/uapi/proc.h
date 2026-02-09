#pragma once
#include <stdint.h>

typedef uint8_t pid_t;

enum proc_state : uint8_t {
    BLOCKED,
    READY,
    DEAD,
    UNALLOCATED
};

struct procinfo {
    pid_t upid;
    enum proc_state state;
};

