#ifndef GC_H
#define GC_H

#include <stddef.h>

#include "scope.h"

typedef struct GCObjectSlot GCObjectSlot;

typedef struct GCObject GCObject;

typedef enum MemoryType {
    MEM_SCOPE,
    MEM_DICT,
    MEM_FUNCTION,
    MEM_OBJECT,
    MEM_STRING,
    MEM_RAW
} MemoryType;

/**
 * Allocate memory and register it in the GC
 */
void* gc_malloc(size_t size);

/**
 * Force free memory, nethertheless if its somewhere reference or not
 */
void gc_free(void* ptr);

/**
 * Starts a mark procedure
 */
void gc_mark(void* root, MemoryType memory_type);

/**
 * Starts a sweep procedure and resets marked objects
 */
void gc_sweep();

/**
 * Starts a mark and sweep procedure
 */
void gc_collect(void* root, MemoryType memory_type);

#endif //GC_H