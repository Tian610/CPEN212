#include <assert.h>
#include <stdio.h>
#include "cpen212alloc.h"
#include "cpen212common.h"

int cpen212_debug(void *heap, int op) {
    if (!heap) {
        printf("Heap is NULL.\n");
        return 0;
    }

    heap_state *h = (heap_state *) heap;

    printf("\n===== HEAP DEBUGGER =====\n");
    printf("Heap Start:\033[0;36m   %p\033[0m\n", h->heap_start);
    printf("Heap End:\033[0;36m     %p\033[0m\n\n", h->heap_end);
    printf("Heap Size:\033[0;36m     %ld\033[0m\n\n", h->heap_end - h->heap_start);

    printf("======= FREE LIST =======\n");
    int size = 0;
    block *current = (block *) h->heap_start;
    
    while ((char *)current < (char *)h->heap_end) {
        
        // Check if the block is allocated or free based on LSB of size
        if (!(current->size & 1)) {
        
            printf("Block at \033[0;35m%p\033[0m\n", current);
            printf("   Size: \033[0;36m%lu\033[0m\n", current->size & ~1);  // Size without LSB
            printf("  Total: \033[0;36m%lu\033[0m\n", sizeof(block) + (current->size & ~1));
            size += (current->size & ~1)+ sizeof(block);
        }

        // Move to the next block (skip current block size + block header)
        current = (block *)((char *)current + sizeof(block) + (current->size & ~1));
    }

    printf("======= FULL LIST =======\n");
    current = (block *) h->heap_start;

    size = 0;

    // Scan through the heap to print all blocks
    while ((char *)current < (char *)h->heap_end) {
        printf("Block at \033[0;35m%p\033[0m\n", current);
        
        // Check if the block is allocated or free based on LSB of size
        if (current->size & 1) {
            printf("  \033[0;31mAllocated\033[0m\n");
        } else {
            printf("  \033[0;32mFree\033[0m\n");
        }

        printf("   Size: \033[0;36m%lu\033[0m\n", current->size & ~1);  // Size without LSB
        printf("  Total: \033[0;36m%lu\033[0m\n", sizeof(block) + (current->size & ~1));
        size += (current->size & ~1)+ sizeof(block);

        // Move to the next block (skip current block size + block header)
        current = (block *)((char *)current + sizeof(block) + (current->size & ~1));
    }

    printf("=========================\n");
        printf("     Total Size: \033[0;36m%d\033[0m\n", size);
        printf("  Expected Size: \033[0;36m%ld\033[0m\n", h->heap_end-h->heap_start);
    printf("=========================\n\n\n");
    return 0;
}