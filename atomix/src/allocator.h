#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <inttypes.h>

extern size_t allocated_memory;
extern size_t freed_memory;

void* js_malloc(size_t size);

void* js_calloc(size_t num, size_t size);

void js_free(void* ptr);

#endif //ALLOCATOR_H
