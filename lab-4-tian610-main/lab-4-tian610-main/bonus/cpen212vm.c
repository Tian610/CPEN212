#include "cpen212vm.h"

void *vm_init(void *physmem, size_t num_phys_pages, FILE *swap, size_t num_swap_pages) {
    // YOUR CODE HERE
}

vm_result_t vm_translate(void *vm, paddr_t pt, vaddr_t addr, access_type_t access, bool user) {
    // YOUR CODE HERE
}

vm_result_t vm_new_addr_space(void *vm, asid_t asid) {
    // YOUR CODE HERE
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
