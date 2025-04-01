#include "allocator.h"

#include <stdlib.h>
#include <string.h>

size_t allocated_memory = 0;
size_t freed_memory = 0;

#define TRACK_MEMORY


#ifdef TRACK_MEMORY
void* js_malloc(size_t size)
{
    size += sizeof(size_t);
    size_t* ptr = malloc(size);
    allocated_memory += size;
    ptr[0] = size;
    return ptr + 1;
}

void* js_calloc(size_t num, size_t size)
{
    size_t total_size = num * size + sizeof(size_t);
    size_t* ptr = malloc(total_size);
    if (!ptr) return NULL;

    allocated_memory += total_size;
    ptr[0] = total_size;

    void* user_ptr = ptr + 1;
    memset(user_ptr, 0, num * size);

    return user_ptr;
}

void js_free(void* ptr)
{
    if (!ptr)
    {
        return;
    }

    size_t* p = (size_t*)ptr - 1;
    freed_memory += p[0];
    free(p);
}
#else
inline void* js_malloc(size_t size)
{
    return malloc(size);
}

inline void* js_calloc(size_t num, size_t size)
{
    return calloc(enum, size);
}

inline void js_free(void* ptr)
{
    return free(ptr);
}
#endif
