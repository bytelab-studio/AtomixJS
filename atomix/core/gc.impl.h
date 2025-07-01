#ifndef GC_IMPL_H
#define GC_IMPL_H

#include "gc.h"

#include <inttypes.h>

#define GC_SLOT_COUNT 8

struct GCObjectSlot {
    int marked;
    void* ptr;
};

struct GCObject
{
    struct GCObject* next;
    struct GCObject* prev;
    uint32_t full_slots;
    GCObjectSlot slots[GC_SLOT_COUNT];
};

#endif //GC_IMPL_H