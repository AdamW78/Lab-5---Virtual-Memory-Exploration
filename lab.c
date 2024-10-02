#define _GNU_SOURCE
#include "util.h"
#include <stdio.h>      // for printf
#include <stdlib.h>     // for atoi (and malloc() which you'll likely use)
#include <sys/mman.h>   // for mmap() which you'll likely use
#include <stdalign.h>

alignas(4096) volatile char global_array[4096 * 32];

void labStuff(int which) {
    if (which == 0) {
        /* do nothing */
    } else if (which == 1) {
        // Change the code in lab.c’s labStuff so that when the parameter is 1, it runs some code to write to the array.
        for (int i = 0; i < 4096 * 32; i++) {
            global_array[i] = 'a';
        }

    } else if (which == 2) {
        alignas (4096) volatile char* allocation = (volatile char *) malloc(1024 * 1024);
        for (int i = 0; i < 1024; i++) {
            char access = allocation[i];
            if (access == 0) {
                __asm__(""); // force the compiler to not optimize this away
            } else {
                __asm__(" "); // force the compiler to not optimize this away
            }
        }
    } else if (which == 3) {
        alignas (4096) volatile char* allocation = (volatile char *) malloc(1024 * 1024);
        for (int i = 0; i < 4096 * 32; i += 4096) {
            allocation[i] = 'a';
        }

    } else if (which == 4) {
        char *ptr;
        long heap_address = 0x5555555bbfff;
        long alloc_location = heap_address + 0x200000;
        long remainder = alloc_location % 4096;
        long page_aligned_alloc_location = alloc_location;
        if (remainder != 0) {
            page_aligned_alloc_location = alloc_location - remainder + 4096;
        }
        ptr = mmap((void*) page_aligned_alloc_location /* hint address */,
                4096 /* length */,
                PROT_READ | PROT_WRITE,
                MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE,
                -1, /* file descriptor (-1 for "none") */
                0
        );
        if (ptr == MAP_FAILED) { printf("Failure!"); }
        for (int i = 0; i < 4096; i++) {
            ptr[i] = 'a';
        }
    } else if (which == 5) {
        char *ptr;
        long heap_address = 0x5555555bbfff;
        long alloc_location = heap_address + 0x10000000000;
        long remainder = alloc_location % 0x1000;
        long page_aligned_alloc_location = alloc_location;
        if (remainder != 0) {
            page_aligned_alloc_location = alloc_location - remainder + 0x1000;
        }
        ptr = mmap((void*) page_aligned_alloc_location /* hint address */,
                0x1000 /* length */,
                PROT_READ | PROT_WRITE,
                MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE,
                -1, /* file descriptor (-1 for "none") */
                0
        );
        if (ptr == MAP_FAILED) { printf("Failure!"); }
        for (int i = 0; i < 0x1000; i++) {
            ptr[i] = 'a';
        }
    }
}

int main(int argc, char **argv) {
    int which = 0;
    if (argc > 1) {
        which = atoi(argv[1]);
    } else {
        fprintf(stderr, "Usage: %s NUMBER\n", argv[0]);
        return 1;
    }
    printf("Memory layout:\n");
    print_maps(stdout);
    printf("\n");
    printf("Initial state:\n");
    force_load();
    struct memory_record r1, r2;
    record_memory_record(&r1);
    print_memory_record(stdout, NULL, &r1);
    printf("---\n");

    printf("Running labStuff(%d)...\n", which);

    labStuff(which);

    printf("---\n");
    printf("Afterwards:\n");
    record_memory_record(&r2);
    print_memory_record(stdout, &r1, &r2);
    print_maps(stdout);
    return 0;
}
