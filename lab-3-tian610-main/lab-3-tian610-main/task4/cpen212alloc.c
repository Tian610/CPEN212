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

    while ((void *) current < heap->heap_end) {                               // Iterate through every free block

        size_t actual_size = current->size & ~1;

        if (current->size >= aligned_sz && (current->size & 1) == 0) {
            
            if (actual_size >= aligned_sz + sizeof(block)) {                                // If space can fit both our allocation and another header, we split the free spot
                block *new = (block *)((char *)current + sizeof(block) + aligned_sz);       // Find address of new free block
                new->size = (actual_size - aligned_sz - sizeof(block)) & ~1;                // Set its size to be whats left of the original free block
                cpen212_free(heap_handle, ((char *) new + sizeof(block)));
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
    if (!heap_handle || !p) {
        return;
    }

    heap_state *heap = (heap_state *)heap_handle;
    block *current = (block *)heap->heap_start;
    block *prev = NULL;

    while ((char *)current < (char *)heap->heap_end) {
        if ((char *)current + sizeof(block) == p) {
            current->size &= ~1; // Mark block as free

            // Find next block
            block *next = (block *)((char *)current + sizeof(block) + (current->size & ~1));

            // Case 1: Coalesce with next block if it's free
            if ((char *)next < (char *)heap->heap_end && (next->size & 1) == 0) {
                current->size += next->size + sizeof(block);
            }

            // Case 2: Coalesce with previous block if it's free
            if (prev && (prev->size & 1) == 0) {
                prev->size += current->size + sizeof(block);
            }

            return; // Successfully freed
        }

        prev = current;
        current = (block *)((char *)current + sizeof(block) + (current->size & ~1));
    }
}


void *cpen212_realloc(void *s, void *prev, size_t nbytes) {

    // A few scenarios to consider here:
    // 1. If nbytes is smaller, we reallocate in place and free what's left, coalescing
    // 2. Nbytes is larger, and we have a free block ahead
    //    If the free block ahead is large enough, simply take it over.
    //    If not large enough, we allocate in new position and free the old one.
    // 3. Nbytes is larger, and we have a free block behind
    //    If large enough, we copy everything backwards to the old position
    //    If not large enough, we allocate in new position and free the old one.
    // 4. Nbytes is larger, and we have a forward and backwards free block.
    //    Copy everything to the start if possible
    //    If not large enough, we allocate in new position and free the old one.

    // If prev is NULL, just call alloc...
    if (!prev) {
        return cpen212_alloc(s, nbytes);
    }

    if (nbytes == 0) {
        cpen212_free(s, prev);
        return NULL;
    }

    heap_state *heap = (heap_state *)s;
    block* curr = (block *) (prev - sizeof(block));

    size_t actual_size = curr->size & ~1;
    
    size_t aligned_sz = (nbytes + 7) & ~7;

    // Return itself if nbytes is same as size (no realloc needed)
    if (nbytes == actual_size) {
        return prev;
    }

    // 1. Nbytes is smaller:
    // Code is mostly taken straight from alloc, it's basically just equivalent to splitting a free block
    if (aligned_sz < curr->size) {
        if (actual_size >= aligned_sz + sizeof(block)) {
            block *new = (block *)((char *)curr + sizeof(block) + aligned_sz);       // Find address of new free block
            new->size = (actual_size - aligned_sz - sizeof(block)) & ~1;             // Set its size to be whats left of the original free block
            cpen212_free(s, ((char *) new + sizeof(block)));
            if (new->size < 1) {
                aligned_sz = curr->size;
            }
        }

        curr->size = aligned_sz | 1;                                                 // Change current's size to the allocated size
        return ((char *)curr + sizeof(block));  
    }

    // The other cases

    // Locate adjacent blocks
    block *next = (block *)((char *)curr + sizeof(block) + actual_size);
    block *prev_block = NULL;
    block *temp = (block *)heap->heap_start;

    // Find previous if it does exist
    while ((char *) temp < (char *)heap->heap_end) {
        block *potential_next = (block *)((char *)temp + sizeof(block) + (temp->size & ~1));
        if (potential_next == curr) {
            prev_block = temp;
            break;
        }
        temp = potential_next;
    }

    // Attempt to merge with adjacent blocks if there is enough space

    char *newStart = NULL;
    size_t newSize = 0;

    // 2. Check if next block is big enough
    if (((next->size & 1) == 0) && (actual_size + next->size + sizeof(block) >= aligned_sz)) {
        newStart = (char *)curr;
        newSize = actual_size + next->size + sizeof(block);
    }
    // 3. Check if prev block is big enough
    else if (prev_block && ((prev_block->size & 1) == 0) &&
             (prev_block->size + actual_size + sizeof(block) >= aligned_sz)) {
        newStart = (char *)prev_block;
        newSize = prev_block->size + actual_size + sizeof(block);
    }
    // Check if both are big enough
    else if (prev_block && ((prev_block->size & 1) == 0) &&
             ((next->size & 1) == 0) &&
             (prev_block->size + actual_size + next->size + 2 * sizeof(block) >= aligned_sz)) {
        newStart = (char *)prev_block;
        newSize = prev_block->size + actual_size + next->size + 2 * sizeof(block);
    }

    if (newStart) {
        block *newBlock = (block *)newStart;
        newBlock->size = aligned_sz | 1;

        if (newBlock->size + newStart + sizeof(block) >  (char *) heap->heap_end) {
            return NULL;
        }

        memmove(newStart + sizeof(block), prev, actual_size);

        block *newFree = (block *)(newStart + sizeof(block) + aligned_sz);
        newFree->size = (newSize - aligned_sz - sizeof(block)) & ~1;
        if (newFree->size == 0) {
            newBlock->size += sizeof(block);
        }
        cpen212_free(s, (char *)newFree + sizeof(block));

        return newStart + sizeof(block);
    }

    // Case 3: Allocate new space and copy
    char *newAlloc = (char *)cpen212_alloc(s, aligned_sz);
    if (!newAlloc) return NULL;  // Allocation failed

    memcpy(newAlloc, prev, actual_size);
    cpen212_free(s, prev);
    return newAlloc;    
}