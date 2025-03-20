#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "cpen212vm.h"

typedef struct {
    uint32_t valid      : 1;                    // PTE[0]  - valid
    uint32_t present    : 1;                    // PTE[1]  - present
    uint32_t readable   : 1;                    // PTE[2]  - readable
    uint32_t writable   : 1;                    // PTE[3]  - writable
    uint32_t executable : 1;                    // PTE[4]  - executable
    uint32_t user       : 1;                    // PTE[5]  - user
    uint32_t accessed   : 1;                    // PTE[6]  - accessed
    uint32_t reserved   : 5;                    // PTE[7:11] - reserved (must be 0)
    uint32_t ppn        : 20;                   // PTE[12:31] - physical page number (ppn)
} pte_t;

typedef struct {
    void *physmem;                              // Pointer to the region of physical memory
    size_t num_phys_pages;                      // Total number of physical pages
    FILE *swap;                                 // Swap file for swapping pages to disk (if applicable)
    size_t num_swap_pages;                      // Number of pages in swap space
    void *freelist;                             // stores the next free page
} vm_t;

typedef struct {
    void* prevPage;                             // If null, start of list
    void* nextPage;                             // If null, end of list
} freePage;

static const char *msg(bool passed) {
    return passed ? "\033[0;32mPASSED\033[0m" : "\033[0;31mFAILED\033[0m";
}

void cpen212_debug(void *vm_) {
    if (!vm_) {
        printf("Heap is NULL.\n");
        return;
    }

    vm_t *vm = (vm_t *) vm_;

    printf("\n===== VM DEBUGGER =====\n");
    printf("Physical Memory Location:\033[0;36m   %p\033[0m\n", vm->physmem);
    printf("Number of Physical Pages:\033[0;36m   %ld\033[0m\n", vm->num_phys_pages);
    printf("Number of Swap Pages:\033[0;36m    %ld\033[0m\n\n", vm->num_swap_pages);

    printf("======= FREE LIST =======\n");
    freePage *current = vm->freelist;
    
    while (current) {
        printf("Free Page at: %p\n", current);
        current = current->nextPage;
    }

    //printf("======= FULL LIST =======\n");
    // current = (void *) (vm + 0x1000);

    // // Scan through the heap to print all blocks
    // while (current < (void *) (vm->physmem + vm->num_phys_pages * 4096)) {
    //     printf("Free Page at: %p\n", current);
    //     current = current + 0x1000;
    // }
    return;
}

int main(int argc, char **argv) {
    vm_result_t result;
    vm_status_t result2;

    uint8_t *physmem = aligned_alloc(0x1000, 0x7000);
    assert(physmem);

    clock_t start_time = clock();

    void *vm_handle = vm_init(physmem, 7, NULL, 0);

    clock_t end_time = clock();
    double time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Time elapsed for init: %f ms\n", time_taken);

    // Tests

    result = vm_new_addr_space(vm_handle, 0);
    printf("status %s  address %s\n", msg(result.status == VM_OK), msg(result.addr == 0x1000));

    paddr_t pt = result.addr;

    result2 = vm_map_page(vm_handle, pt, 0x401001, true, true, true, true);
    printf("status %s\n", msg(result2 == VM_OK));

    result = vm_translate(vm_handle, pt, 0x401001, VM_WRITE, true);
    printf("status %s  address %s\n", msg(result.status == VM_OK), msg(result.addr == 0x3001));

    result2 = vm_map_page(vm_handle, pt, 0x402007, true, true, true, false);
    printf("status %s\n", msg(result2 == VM_OK));

    result = vm_translate(vm_handle, pt, 0x402007, VM_READ, true);
    printf("status %s  address %s\n", msg(result.status == VM_BAD_PERM), msg(result.addr == 0));

    result = vm_translate(vm_handle, pt, 0x402007, VM_WRITE, true);
    printf("status %s  address %s\n", msg(result.status == VM_OK), msg(result.addr == 0x4007));

    result = vm_new_addr_space(vm_handle, 0);
    printf("status %s  address %s\n", msg(result.status == VM_OK), msg(result.addr == 0x5000));

    result2 = vm_map_page(vm_handle, pt, 0x402007, true, true, true, false);
    printf("status %s\n", msg(result2 == VM_DUPLICATE));

    paddr_t pt2 = result.addr;

    result2 = vm_map_page(vm_handle, pt2, 0x401001, true, true, true, false);
    printf("status %s\n", msg(result2 == VM_OUT_OF_MEM));

    // Only 1 free page here, we will unmap a page, should be 2 free pages.
    result2 = vm_unmap_page(vm_handle, pt, 0x401001);
    printf("status %s\n", msg(result2 == VM_OK));

    // Now remove the other free page from asid 0, which should free 2 more pages

    result2 = vm_unmap_page(vm_handle, pt, 0x402007);
    printf("status %s\n", msg(result2 == VM_OK));
    cpen212_debug(vm_handle);

    free(physmem);
}
