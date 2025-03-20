#include <stdlib.h>
#include <string.h>
#include "cpen212alloc.h"
#include "cpen212common.h"

#define BLOCK_SIZE 7

// CHANGE THIS FILE to implement your heap allocator for each task


 /*
  * Heap structure is as follows from heap_start:
  *
  * heap_start: 16 bytes
  *   heap_end: 16 bytes
  *       data: all bytes up to heap_end
  * 
 */
void *cpen212_init(void *heap_start, void *heap_end) {
    if (!heap_start || !heap_end || heap_start >= heap_end) {
        return NULL;  // Invalid heap range
    }

    heap_state *heap = (heap_state*)heap_start; // Creates the heap at heap_start

    heap->heap_start = heap_start + 2*sizeof(void *);            
    heap->heap_end = heap_end;                  // Sets end of heap

    block *first = (block*) heap->heap_start;

    first->size = heap->heap_end - heap->heap_start - sizeof(block);

    return heap;                                // return pointer to heap
                                                // As long as two heaps do not overlap. Different heap pointers 
                                                // means we should be able to run concurrent heaps... I hope...
}

void *cpen212_alloc(void *heap_handle, size_t nbytes) {
    // Don't allocate if user is dumb;
    if (!heap_handle || nbytes == 0) {
        return NULL;
    }

    // Cast the heap_handle to a heap_state to retrieve metadata
    heap_state *heap = (heap_state *)heap_handle;

    // Calculate the aligned size of the block (granularity is currently 8)
    size_t aligned_sz = (nbytes + BLOCK_SIZE) & ~BLOCK_SIZE;

    block *current = (block*) heap->heap_start;     // Grab the first block on the free list

    while (current) {                               // Iterate through every free block

        size_t actual_size = current->size & ~1;

        if (current->size >= aligned_sz && (current->size & 1) == 0) {
            
            if (actual_size >= aligned_sz + sizeof(block)) {  // TODO: This can currently create free blocks of size 0                               // If space can fit both our allocation and another header, we split the free spot
                block *new = (block *)((char *)current + sizeof(block) + aligned_sz);       // Find address of new free block
                new->size = (actual_size - aligned_sz - sizeof(block)) & ~1;                // Set its size to be whats left of the original free block

                if (new->size < 1) {
                    aligned_sz = current->size;
                }
            }

            current->size = aligned_sz | 1;                                                 // Change current's size to the allocated size
            return ((char *)current + sizeof(block));                                       // Return the memory address after the block header
        }

        current = (block*) ((char*) current + (current->size & ~1) + sizeof(block));
    }

    return NULL;                                                                            // Return NULL if no free block found
} 

void cpen212_free(void *heap_handle, void *p) {

    // Cast the heap_handle to a heap_state to retrieve metadata
    heap_state *heap = (heap_state *)heap_handle;

    block *current = (block*) heap->heap_start;
    while ((char*)current < (char*)heap->heap_end) {

        if ((char*)current + sizeof(block) == p) {
                current->size &= ~1;                                                         // Set LSB to 0
                return;
            }

        current = (block *) ((char*) current + (current->size & ~1) + sizeof(block));        // Increment to next block
    }
    return;
}

void *cpen212_realloc(void *s, void *prev, size_t nbytes) {

    // If prev is NULL, just call alloc...
    if (!prev) {
        return cpen212_alloc(s, nbytes);
    }


    // Basic idea. If we're making it smaller, we can simply copy everything up to nbytes, free the block, then reallocate with smaller size
    // If we're making it larger, employ a lookahead to see if there is a free block ahead large enough. If yes, take over it.
    // If not, then we copy the contents and move it to the next available free block.
    // Should be able to use alloc and free to accomplish most of these tasks, although i guess that's less efficient, but i dont care

    // Turns out i dont read and i dont have to make an actually good realloc for task2... 

    block *p = (block *) prev;
    void *new = cpen212_alloc(s, nbytes);

    memcpy(new, prev, nbytes);

    cpen212_free(s, prev);

    return (void *) new;
}