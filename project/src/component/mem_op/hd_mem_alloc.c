/*
    FILE NAME: hd_mem_alloc.c
    PURPOSE: Implementation of memory allocation functions
    DATE: 2024/10/18
    AUTHOR: lan-tianxiang
*/

#include "hd_mem_alloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jemalloc/jemalloc.h>  // Include jemalloc header

// Allocate memory
void* mem_alloc(size_t size) {
    void* ptr = je_malloc(size);  // Use jemalloc's malloc
    if (!ptr) {
        printf("Memory allocation failed\n");
        return NULL;
    }
    return ptr;
}

// Free memory
void mem_free(void* ptr) {
    if (ptr) {
        je_free(ptr);  // Use jemalloc's free
    }
}

// Reallocate memory
void* mem_realloc(void* ptr, size_t size) {
    void* new_ptr = je_realloc(ptr, size);  // Use jemalloc's realloc
    if (!new_ptr) {
        printf("Memory reallocation failed\n");
        return NULL;
    }
    return new_ptr;
}

// Copy memory
void* mem_copy(void* dest, const void* src, size_t size) {
    return memcpy(dest, src, size);
}

// Set memory
void* mem_set(void* dest, int value, size_t size) {
    return memset(dest, value, size);
}

// Compare memory
int mem_cmp(const void* ptr1, const void* ptr2, size_t size) {
    return memcmp(ptr1, ptr2, size);
}

// Move memory
void* mem_move(void* dest, const void* src, size_t size) {
    return memmove(dest, src, size);
}