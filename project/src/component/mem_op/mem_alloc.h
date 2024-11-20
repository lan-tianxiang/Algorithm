#ifndef MEM_ALLOC_H
#define MEM_ALLOC_H

void* mem_alloc(size_t size);

void mem_free(void* ptr);

void* mem_realloc(void* ptr, size_t size);

void* mem_copy(void* dest, const void* src, size_t size);

void* mem_set(void* dest, int value, size_t size);

int mem_cmp(const void* ptr1, const void* ptr2, size_t size);

void* mem_move(void* dest, const void* src, size_t size);

void* mem_calloc(size_t num, size_t size);

#endif // MEM_ALLOC_H