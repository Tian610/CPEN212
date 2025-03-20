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
    void *physmem;                              // Pointer to the region of physical memory
    size_t num_phys_pages;                      // Total number of physical pages
    FILE *swap;                                 // Swap file for swapping pages to disk (if applicable)
    size_t num_swap_pages;                      // Number of pages in swap space
    void *freelist;                             // stores the next free page
    int asidlist[512];                        // Stores all ASIDs
} vm_t;

typedef struct {
    void* prevPage;                             // If null, start of list
    void* nextPage;                             // If null, end of list
} freePage;

void *vm_init(void *physmem, size_t num_phys_pages, FILE *swap, size_t num_swap_pages) {
    if (!physmem || num_phys_pages < 4 || num_phys_pages > 1048576) {
        return NULL;                            // Ensure invariants aren't violated
    }

    vm_t* vm = (vm_t *) physmem;
    vm->physmem = physmem;                      // Assign the provided physical memory pointer
    vm->num_phys_pages = num_phys_pages;        // Set the number of physical pages
    vm->swap = swap;                            // Set the swap file pointer (if any)
    vm->num_swap_pages = num_swap_pages;        // Set the number of swap pages

    freePage* prevPage = NULL;
    for (size_t i = 1; i < num_phys_pages; i++) {   // Skip page 0
        freePage* currentPage = 
            (freePage*) (physmem + i * PAGE_SIZE);  // Point to current page

        currentPage->prevPage = prevPage;       // Set previous page pointer
        if (prevPage != NULL) {
            prevPage->nextPage = currentPage;   // Set the next page pointer of the previous page
        }

        currentPage->nextPage = NULL;           // Set next page pointer to NULL (end of list)
        prevPage = currentPage;                 // Update prevPage for next iteration
    }

    vm->freelist = (physmem + PAGE_SIZE);

    return physmem;
    
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

    // Page tables begin at physmem + 4096 (1st page reserved for implementation)
    // Every Address space is given a physical address and contains a page table of pte's
    // Each PTE points to a second level page table
    // Each second level PTE points to a unique page

    // So, we begin by checking against an array containg bits which check if a page is allocated
    // Then, find an unallocated page and turn it into a top level page table
    // Then, we simply return the physical address of this page.

    // CHAT I GOT IT WE CAN USE A EXPLICIT FREELIST OMG

    vm_t* vm = (vm_t *) vm_;                    // Cast the vm_ handle

    void* newPage = NULL;

    if (vm->freelist) {                         // Grab the first available free page, if none are available,
        newPage = vm->freelist;                 // return an out_of_mem. CHANGE WHEN SWAP EXISTS
    } else {
        vm_result_t result = {VM_OUT_OF_MEM, 0};
        return result;
    }

    vm->freelist = ((freePage *)vm->freelist)->nextPage;      // Set the freelist to the next available free page
    ((freePage *) vm->freelist)->prevPage = NULL;

    memset(newPage, 0, PAGE_SIZE);              // Initialize all entries to 0

    vm->asidlist[asid] = (newPage - vm->physmem) / 4096; // Store address of asid

    vm_result_t result = {VM_OK, newPage - vm->physmem}; // Find the physical address

    return result;                              // Return the status

}

vm_status_t vm_destroy_addr_space(void *vm_, asid_t asid) {
    vm_t* vm = (vm_t *) vm_;                    // Cast the vm_ handle

    if (!vm->asidlist[asid]) {
        return VM_BAD_ADDR;
    }

    void* pageTable1 = (vm->asidlist[asid] * 4096) + vm->physmem;

    for (pte_t* pte1 = pageTable1; pte1 < (pte_t *) (pageTable1 + 0x1000); pte1 += 1) {
        
        // If we have a valid level 1 entry, we enter the corresponding level 2 page table
        if (pte1->valid) {
            pte_t* pageTable2 = (pte_t *) (vm->physmem + (pte1->ppn << 12));

            // Iterate through every 2nd level page table entry
            for (pte_t* pte2 = pageTable2; pte2 < (pte_t *) (pageTable2 + 0x1000); pte2 = pte2 + 1) {
        
                // If we have a valid level 2 entry, we free the corresponding page 
                if (pte2->valid) {
                    freePage* pa = (freePage *) ((pte2->ppn << 12) + vm->physmem);
                    pa->nextPage = vm->freelist;
                    vm->freelist = pa;
                }
            }

            // Now, deallocate the second level page table

            freePage* pa = (freePage *) pageTable2;
            pa->nextPage = vm->freelist;
            vm->freelist = pa;
        }
    }

    // Finally, deallocate the ASID

    freePage* pa = (freePage *) pageTable1;
    pa->nextPage = vm->freelist;
    vm->freelist = pa;

    return VM_OK;
    
}

vm_status_t vm_map_page(void *vm_, paddr_t pt, vaddr_t addr, bool user, bool exec, bool write, bool read) {
    vm_t* vm = (vm_t *) vm_;                    // Cast the vm_ handle
    
    // Take pt to find first level page table, if it exists
    // Check Virtual address and find second level page table
    // If mapping to second level page table doesn't exist, create it
    // else go to the second level page table

    size_t l1 = ((addr >> 22) & 0x3FF) * 4;         // Top-level index (10 bits)
    size_t l2 = ((addr >> 12) & 0x3FF) * 4;         // Second-level index (10 bits)

    pte_t* pt1 = (pte_t *) (vm->physmem + pt + l1); // Add our page table address, offset from 0, and the entry for the page table to get address
    void* newPage = NULL;

    // If the first level page table entry exists...
    if (pt1->valid) {
        pte_t* pt2 = (pte_t *) (vm->physmem + (pt1->ppn << 12) + l2);

        // If the second level page table entry exists...
        if (pt2->valid) {
            // Then the virtual address is already mapped
            // and we return a VM_DUPLICATE

            return VM_DUPLICATE;
        }

        // If we're here, then there exists no second level page table entry.
        // So find a free page, allocate it, and create the entry.

        if (vm->freelist) {                         // Grab the first available free page, if none are available,
            newPage = vm->freelist;                 // return an out_of_mem. CHANGE WHEN SWAP EXISTS
        } else {
            return VM_OUT_OF_MEM;
        }

        vm->freelist = ((freePage *)vm->freelist)->nextPage;      // Set the freelist to the next available free page

        *pt2 = (pte_t) {1, 1, read, write, exec, user, 0, 0, (newPage - vm->physmem) >> 12};

        return VM_OK;
    }

    // If we're here, then the first level page table entry doesn't exist.
    // Allocate a new second level page table and a new page.

    if (vm->freelist && ((freePage *)vm->freelist)->nextPage) { // Grab the first available free page, if none are available,
        newPage = vm->freelist;                                 // return an out_of_mem. CHANGE WHEN SWAP EXISTS
    } else {
        return VM_OUT_OF_MEM;
    }

    vm->freelist = ((freePage *)vm->freelist)->nextPage;      // Set the freelist to the next available free page

    *pt1 = (pte_t) {1, 1, 0, 0, 0, 0, 0, 0, (newPage - vm->physmem) >> 12};

    // Now that a page table 1 entry exists, we create a page table 2 entry

    pte_t* pt2 = (pte_t *) (vm->physmem + (pt1->ppn << 12) + l2);

    newPage = vm->freelist;

    vm->freelist = ((freePage *)vm->freelist)->nextPage;      // Set the freelist to the next available free page
    ((freePage *) vm->freelist)->prevPage = NULL;

    *pt2 = (pte_t) {1, 1, read, write, exec, user, 0, 0, (newPage - vm->physmem) >> 12};

    return VM_OK;
}

vm_status_t vm_unmap_page(void *vm_, paddr_t pt, vaddr_t addr) {
    vm_t* vm = (vm_t *) vm_;                        // Cast the vm_ handle

    size_t l1 = ((addr >> 22) & 0x3FF) * 4;         // Top-level index (10 bits)
    size_t l2 = ((addr >> 12) & 0x3FF) * 4;         // Second-level index (10 bits)

    pte_t* pte1 = (pte_t *) (vm->physmem + pt + l1); // Add our page table address, offset from 0, and the entry for the page table to get address

    if (!pte1->valid) {                                        // Check if valid
        return VM_BAD_ADDR;
    }

    if (pte1->valid && !pte1->present) {              // TEMP: Only cause we dont have disk access yet
        return VM_BAD_IO;
    }

    pte_t* pte2 = (pte_t *) (vm->physmem + (pte1->ppn << 12) + l2);

    if (!pte2->valid) {                              // Check if valid
        return VM_BAD_ADDR;
    }

    if (pte2->valid && !pte2->present) {              // TEMP: Only cause we dont have disk access yet
        return VM_BAD_IO;
    }

    pte2->valid = 0;

    freePage* pa = (freePage *) ((pte2->ppn << 12) + vm->physmem);

    pa->nextPage = vm->freelist;                    // Page is freed?
    vm->freelist = pa;

    // Check if the 2nd level page table is empty, if yes, free it.
    void *pagetable2 = (vm->physmem + (pte1->ppn << 12));
    
    for (int i = 0; i < 512; i++) {
        if (((pte_t *)pagetable2)[i].valid) {
            return VM_OK;
        }
    }

    pa = (freePage *) pagetable2;
    pa->nextPage = vm->freelist;                    // Page is freed?
    vm->freelist = pa;

    return VM_OK;
}
