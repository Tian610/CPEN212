#include "cpen212vm.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define PAGE_SIZE 4096

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
    paddr_t page_table[1024];                   // Array for top-level page table (1024 entries)
    size_t num_pages;                           // Number of pages in this address space
} addr_space_t;

typedef struct {
    void *physmem;                              // Pointer to the region of physical memory
    size_t num_phys_pages;                      // Total number of physical pages
    FILE *swap;                                 // Swap file for swapping pages to disk (if applicable)
    size_t num_swap_pages;                      // Number of pages in swap space
    addr_space_t *address_spaces[512];          // Supports 512 address spaces, each with its own page table
} vm_t;

void *vm_init(void *physmem, size_t num_phys_pages, FILE *swap, size_t num_swap_pages) {
    if (!physmem || num_phys_pages < 4 || num_phys_pages > 1048576) {
        return NULL;                            // Ensure invariants aren't violated
    }

    void* vm_handle = physmem;

    if(!vm_handle) {                            // If initialization fails, return NULL
        return NULL;
    }

    vm_t* vm = (vm_t *) vm_handle;
    vm->physmem = physmem;                      // Assign the provided physical memory pointer
    vm->num_phys_pages = num_phys_pages;        // Set the number of physical pages
    vm->swap = swap;                            // Set the swap file pointer (if any)
    vm->num_swap_pages = num_swap_pages;        // Set the number of swap pages

    memset(vm->address_spaces, 0, sizeof(vm->address_spaces)); // Set address space to NULL

    return vm_handle;
    
}

// what the heck why don't I read I ONLY NEED TO IMPLEMENT VM_TRANSLATE FOR TASK 2?
// anyways to the TA enjoy the sad attempt to make vm_new_addr_space 
vm_result_t vm_translate(void *vm_, paddr_t pt, vaddr_t addr, access_type_t access, bool user) {

    vm_t *vm = (vm_t *)vm_;

    if (!vm || pt % PAGE_SIZE != 0) {
        return (vm_result_t){VM_BAD_IO, 0};
    }

    size_t l1 = ((addr >> 22) & 0x3FF) * 4;         // Top-level index (10 bits)
    size_t l2 = ((addr >> 12) & 0x3FF) * 4;         // Second-level index (10 bits)
    size_t pg_off = addr & 0xFFF;                   // Offset within page (12 bits)

    pte_t* pt1 = (pte_t *) (vm->physmem + pt + l1); // Add our page table address, offset from 0, and the entry for the page table to get address

    if ((void *) pt1 > vm->physmem + 4096*vm->num_phys_pages) { // Check if out of range
        vm_result_t result = { VM_BAD_ADDR, 0 };
        return result;
    }

    if ((unsigned long)pt1 % 4 != 0) {                        // Check if 32 bit aligned
        vm_result_t result = { VM_BAD_ADDR, 0 };
        return result;
    }

    if (!pt1->valid) {                                        // Check if valid
        vm_result_t result = { VM_BAD_ADDR, 0 };
        return result;
    }

    if (pt1->valid && !pt1->present) {              // TEMP: Only cause we dont have disk access yet
        vm_result_t result = { VM_BAD_IO, 0 };
        return result;
    }

    pt1->accessed = 1;                              // Assuming pte is okay, we set accessed to true

    pte_t* pt2 = (pte_t *) (vm->physmem + (pt1->ppn << 12) + l2);

    if ((void *) pt2 > vm->physmem + 4096*vm->num_phys_pages) { // Check if out of range
        vm_result_t result = { VM_BAD_ADDR, 0 };
        return result;
    }

    if ((unsigned long)pt2 % 4 != 0) {                        // Check if 32 bit aligned
        vm_result_t result = { VM_BAD_ADDR, 0 };
        return result;
    }

    if (!pt2->valid) {                                        // Check if valid
        vm_result_t result = { VM_BAD_ADDR, 0 };
        return result;
    }

    if (user && !pt2->user) {                       // Check for kernel/user
        vm_result_t result = { VM_BAD_PERM, 0 };
        return result;
    }

    if (access == VM_READ && !pt2->readable) {       // Check for read perms
        vm_result_t result = { VM_BAD_PERM, 0 };
        return result;
    } 

    if (access == VM_WRITE && !pt2->writable) {      // Check for write perms
        vm_result_t result = { VM_BAD_PERM, 0 };
        return result;
    }

    if (access == VM_EXEC && !pt2->executable) {     // Check for exec perms
        vm_result_t result = { VM_BAD_PERM, 0 };
        return result;
    }

    if (pt2->valid && !pt2->present) {              // TEMP: Only cause we dont have disk access yet
        vm_result_t result = { VM_BAD_IO, 0 };
        return result;
    }

    pt2->accessed = 1;

    paddr_t pa = (paddr_t) ((pt2->ppn << 12) + pg_off);

    vm_result_t result = {VM_OK, pa};

    return result;
}

vm_result_t vm_new_addr_space(void *vm_, asid_t asid) {

    vm_t* vm = (vm_t *) vm_;

    if (vm->address_spaces[asid]) {
        vm_result_t result = { VM_OUT_OF_MEM, 0 };  // If the page table already exists, then out of memory?
        return result;
    }

    // Use valid bit to determine if an address space has already been allocated
    // If not allocated, we 

}

vm_status_t vm_destroy_addr_space(void *vm, asid_t asid) {
    // YOUR CODE HERE
}

vm_status_t vm_map_page(void *vm, paddr_t pt, vaddr_t addr, bool user, bool exec, bool write, bool read) {
    // YOUR CODE HERE
}

vm_status_t vm_unmap_page(void *vm, paddr_t pt, vaddr_t addr) {
    // YOUR CODE HERE
}
