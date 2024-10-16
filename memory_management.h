#include "kernel/types.h"
#include "user/user.h"

// function stubs
void * _malloc(int size);
void _free(void *ptr);

// segment of memory struct
struct segment {
    // size of segment:
    int length;
    // whether segment is free or not
    // 1 = free, 0 = blocked
    uint8 free;
    // address of next segment:
    struct segment * nextSeg;
};