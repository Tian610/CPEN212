#ifndef __CPEN212COMMON_H__
#define __CPEN212COMMON_H__

// YOUR CODE HERE
//
// This file is included in cpen212alloc.c and cpen212debug.c,
// so it would be the right place to define data types they both share.

typedef struct {
    void *heap_start;
    void *heap_end;
} heap_state;

typedef struct {
    size_t size;
    // LSB of size determines allocation
    // 0 = free
    // 1 = allocated
} block;

#endif // __CPEN212COMMON_H__
