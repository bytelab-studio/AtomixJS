#include "gc.impl.h"

#include <stdlib.h>

#include "panic.h"

#include "scope.impl.h"
#include "object.impl.h"
#include "value.impl.h"
#include "function.impl.h"
#include "dict.impl.h"

GCObject* root_obj = NULL;

static GCObject* alloc_object()
{
    GCObject* gc_obj = malloc(sizeof(GCObject));
    gc_obj->full_slots = 0;
    gc_obj->next = NULL;
    gc_obj->prev = NULL;
    for (size_t i = 0; i < GC_SLOT_COUNT; i++)
    {
        gc_obj->slots[i].ptr = NULL;
    }

    return gc_obj;
}

static inline void free_object(GCObject* obj)
{
    if (obj->prev)
    {
        obj->prev->next = obj->next;
    }
    if (obj->next)
    {
        obj->next->prev = obj->prev;
    }

    free(obj);
}

static int find_ptr_in_object(void* ptr, GCObject** out)
{
    *out = root_obj;
    size_t i;
    while(*out)
    {
        if ((*out)->full_slots == 0)
        {
            *out = (*out)->next;
            continue;
        }

        for (i = 0; i < GC_SLOT_COUNT; i++)
        {
            if ((*out)->slots[i].ptr == ptr)
            {
                return i;
            }
        }

        *out = (*out)->next;
    }

    *out = NULL;
    return -1;
}

void* gc_malloc(size_t size)
{
    if (!root_obj)
    {
        root_obj = alloc_object();
    }

    void* ptr = malloc(size);
    if (!ptr)
    {
        PANIC("Could not allocate memory");
    }
    
    GCObject* obj = root_obj;
    size_t i;

    while(obj)
    {
        if (obj->full_slots == GC_SLOT_COUNT)
        {
            obj = obj->next;
            continue;
        }
        for (i = 0; i < GC_SLOT_COUNT; i++)
        {
            if (obj->slots[i].ptr)
            {
                continue;
            }

            obj->slots[i] = (GCObjectSlot){
                .ptr = ptr,
                .marked = 0
            };
            obj->full_slots++;
            return ptr;
        }

        obj = obj->next;
    }

    obj = alloc_object();
    obj->next = root_obj;
    root_obj->prev = obj;
    root_obj = obj;

    obj->full_slots++;
    obj->slots[0] = (GCObjectSlot){
        .ptr = ptr,
        .marked = 0
    };

    return ptr;
}

void gc_free(void* ptr)
{
    GCObject* obj = NULL;
    int i = find_ptr_in_object(ptr, &obj);
    if (i < 0)
    {
        PANIC("Pointer is not registered");
    }

    obj->slots[i].ptr = NULL;
    obj->full_slots--;
    free(ptr);

    if (!obj->full_slots)
    {
        free_object(obj);
    }

    return;
}

static void mark_string(JSString*);
static void mark_property(JSProperty*);
static void mark_dict(JSDict*);
static void mark_scope(Scope*);
static void mark_function(JSFunction*);
static void mark_object(JSObject*);

/**
 * Marks a pointer in its GCObject*
 * @returns 1 if the pointer was already marked else 0
 */
static inline int mark_pointer(void* ptr)
{
    GCObject* obj = NULL;
    int i = find_ptr_in_object(ptr, &obj);
    if (i < 0)
    {
        PANIC("Pointer is not registered");
    }

    if (obj->slots[i].marked)
    {
        return 1;
    }

    obj->slots[i].marked = 1;
    return 0;
}

static void mark_string(JSString* string)
{
    if (mark_pointer(string))
    {
        return;
    }
    mark_pointer(string->buff);
}

static void mark_property(JSProperty* prop)
{
    JSValue value;
    JSGSBox* box;

    while(prop)
    {
        if (prop->key)
        {
            mark_pointer(prop->key);
        }
        else
        {
            mark_object(prop->symbol);
        }

        value = prop->value;

        switch (value.type)
        {
        case JS_FUNC:
            mark_function(value.value.as_pointer);
            break;
        case JS_OBJECT:
        case JS_SYMBOL:
            mark_object(value.value.as_pointer);
            break;
        case JS_STRING:
            mark_string(value.value.as_pointer);
            break;
        case JS_GS_BOX:
            box = value.value.as_pointer;
            if (box->getter)
            {
                mark_function(box->getter);
            }
            if (box->setter)
            {
                mark_function(box->setter);
            }
            break;
        }

        prop = prop->next;
    }
}

static void mark_dict(JSDict* dict)
{
    if (mark_pointer(dict))
    {
        return;
    }

    mark_pointer(dict->buckets);

    for (size_t i = 0; i < dict->bucket_count; i++)
    {
        mark_property(dict->buckets[i]);
    }
}

static void mark_scope(Scope* scope)
{
    if (mark_pointer(scope))
    {
        return;
    }

    mark_dict(scope->symbols);

    if (scope->parent)
    {
        mark_scope(scope->parent);
    }
}

static void mark_function(JSFunction* function)
{
    if (mark_pointer(function))
    {
        return;
    }

    mark_object(function->base);

    if (!function->is_native)
    {
        mark_scope(function->scope);
    }
}

static void mark_object(JSObject* object)
{
    if (mark_pointer(object))
    {
        return;
    }

    mark_dict(object->properties);

    if (object->prototype && object->prototype != object)
    {
        mark_object(object->prototype);
    }
}

void gc_mark(void* root, MemoryType memory_type)
{
    switch(memory_type)
    {
    case MEM_SCOPE:
        mark_scope(root);
        break;
    case MEM_DICT:
        mark_dict(root);
        break;
    case MEM_FUNCTION:
        mark_function(root);
        break;
    case MEM_OBJECT:
        mark_object(root);
        break;
    case MEM_STRING:
        mark_string(root);
        break;
    case MEM_RAW:
        mark_pointer(root);
        break;
    }
}

void gc_sweep()
{
    GCObject* obj = root_obj;
    GCObject* curr;
    size_t i;
    while (obj)
    {
        for (i = 0; obj->full_slots && i < GC_SLOT_COUNT; i++)
        {
            if (obj->slots[i].marked)
            {
                obj->slots[i].marked = 0;
                continue;
            }

            free(obj->slots[i].ptr);
            obj->slots[i].ptr = NULL;
            obj->full_slots--;
        }

        curr = obj;
        obj = obj->next;
        if (!curr->full_slots)
        {
            free_object(curr);
        }
    }
}

void gc_collect(void* root, MemoryType memory_type)
{
    gc_mark(root, memory_type);
    gc_sweep();
}
