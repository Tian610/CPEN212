#include "task3.h"

// YOUR TASK 3 SUBMISSION CONSISTS OF THIS FILE

// see task3.h for specification of the functions below

unsigned long check_1(const char *arg) {
    unsigned long length = 0;

    while (arg[length] != '\0') {
        length++;
    }

    return (length == 20) ? 1 : 0;
}

unsigned long check_2(const char *arg) {
    unsigned long length = 0;
    
    while (arg[length] != '\0') {
        if (arg[length] > 90 || arg[length] < 65) {
            return 0;
        }
        length++;
    }

    return 1;
}

unsigned long check_3(const char *arg) {
    unsigned long length = 0;

    while (arg[length] != '\0') {
        length++;
    }

    unsigned j = length - 1;

    for (unsigned i = 0; i < length; i++) {
        if (arg[i] != arg[j]) {
            return 0;
        }
        
        j--;
    }

    return 1;
}

unsigned long check_4(const char *arg) {
    unsigned long length = 0;

    while (arg[length] != '\0') {
        length++;
    }
    unsigned long hash = length;

    for (unsigned i = 0; i <= length; i++) {
        hash = ((hash & 0x1) << 8) | (hash & ~(0x1 << 8));
        hash = arg[i] ^ (hash >> 1);
    }
    
    printf("Checksum: %ld \n", hash);

    return (hash == 228) ? 1 : 0;
    
}
