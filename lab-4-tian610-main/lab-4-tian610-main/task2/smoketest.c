#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "cpen212vm.h"

// this is an _extremely basic smoke test_ for VM translation
// to make sure you're not barking up an entirely wrong tree.
// you will need to write your own tests, otherwise
// your code will most likely not work correctly. really.

static const char *msg(bool passed) {
    return passed ? "PASSED" : "FAILED";
}

int main(int argc, char **argv) {
    vm_result_t result;

    uint8_t *physmem = aligned_alloc(0x1000, 0x4000);
    assert(physmem);

    void *vm_handle = vm_init(physmem, 4, NULL, 0);
    for (size_t i = 0x1000; i < 0x3000; ++i) {
        switch (i) {
           case 0x112d: physmem[i] = 0x30; break;
           case 0x112c: physmem[i] = 0x37; break;
           case 0x205d: physmem[i] = 0x10; break;
           case 0x205c: physmem[i] = 0x3f; break;
           default: physmem[i] = 0x00;
        }
    }
    paddr_t top_pt = 0x2000;

    result = vm_translate(vm_handle, top_pt, 0x5c4b539, VM_READ, true);
    printf("status %s  address %s\n", msg(result.status == VM_OK), msg(result.addr == 0x3539));

    result = vm_translate(vm_handle, top_pt, 0x5c4b539, VM_WRITE, false);
    printf("status %s\n", msg(result.status == VM_BAD_PERM));

    result = vm_translate(vm_handle, top_pt, 0x5c4b048, VM_EXEC, true);
    printf("status %s  address %s\n", msg(result.status == VM_OK), msg(result.addr == 0x3048));

    result = vm_translate(vm_handle, top_pt, 0x344b539, VM_READ, false);
    printf("status %s\n", msg(result.status == VM_BAD_ADDR));

    result = vm_translate(vm_handle, top_pt, 0x5c0f539, VM_READ, false);
    printf("status %s\n", msg(result.status == VM_BAD_ADDR));

    free(physmem);
}
