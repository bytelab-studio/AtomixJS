#include "dict.impl.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <gc.h>

#include "function.h"
#include "panic.h"
#include "api.h"

#define HASH_SEED 5381

static uint32_t hash_string(const char* str, size_t bucket_count)
{
    uint32_t hash = HASH_SEED;
    int c;
    while ((c = *str++))
    {
        hash = (hash << 5) + hash + c;
    }
    return hash % bucket_count;
}

JSDict* dict_create_dict(size_t bucket_count)
{
    JSDict* dict = GC_malloc(sizeof(JSDict));
    if (!dict)
    {
        PANIC("Could not allocate memory");
    }
    dict->buckets = GC_malloc(bucket_count * sizeof(JSProperty*));
    dict->bucket_count = bucket_count;
    return dict;
}

int dict_update(JSDict* dict, char* key, JSValue value)
{
    size_t index = hash_string(key, dict->bucket_count);
    JSProperty* entry = dict->buckets[index];

    while (entry)
    {
        if (!entry->key)
        {
            entry = entry->next;
            continue;
        }

        if (strcmp(key, entry->key) == 0)
        {
            entry->value = value;
            return 1;
        }
        entry = entry->next;
    }

    return 0;
}

int dict_update_with_symbol(JSDict* dict, void* symbol, JSValue value)
{
    JSProperty* entry = dict->buckets[0];

    while(entry)
    {
        if (entry->symbol == symbol)
        {
            entry->value = value;
            return 1;
        }
        entry = entry->next;
    }

    return 0;
}

JSValue* dict_get(JSDict* dict, char* key)
{
    size_t index = hash_string(key, dict->bucket_count);
    JSProperty* entry = dict->buckets[index];

    while (entry)
    {
        if (!entry->key) {
            entry = entry->next;
            continue;
        }

        if (strcmp(key, entry->key) == 0)
        {
            return &entry->value;
        }
        entry = entry->next;
    }

    return NULL;
}

JSValue* dict_get_by_symbol(JSDict* dict, void* symbol)
{
    JSProperty* entry = dict->buckets[0];

    while (entry)
    {
        if (entry->symbol == symbol)
        {
            return &entry->value;
        }
        entry = entry->next;
    }

    return NULL;
}

void dict_add(JSDict* dict, char* key, JSValue value) {
    size_t index = hash_string(key, dict->bucket_count);

    JSProperty* new_entry = GC_malloc(sizeof(JSProperty));
    if (!new_entry)
    {
        PANIC("Could not allocate memory");
    }
    new_entry->key = key;
    new_entry->value = value;
    new_entry->next = dict->buckets[index];
    dict->buckets[index] = new_entry;
}

void dict_add_with_symbol(JSDict* dict, void* symbol, JSValue value) {
    JSProperty* new_entry = GC_MALLOC(sizeof(JSProperty));
    if (!new_entry)
    {
        PANIC("Could not allocate memory");
    }
    new_entry->symbol = symbol;
    new_entry->key = NULL;
    new_entry->value = value;
    new_entry->next = dict->buckets[0];
    dict->buckets[0] = new_entry;

}

int dict_delete(JSDict* dict, char* key)
{
    size_t index = hash_string(key, dict->bucket_count);
    JSProperty* entry = dict->buckets[index];

    while (entry)
    {
        if (!entry->key)
        {
            entry = entry->next;
            continue;
        }

        if (strcmp(key, entry->key) == 0)
        {
            if (entry == dict->buckets[index])
            {
                dict->buckets[index] = entry->next;
            }
            else
            {
                JSProperty* prev = dict->buckets[index]->next;
                while (prev->next != entry)
                {
                    prev = prev->next;
                }
                prev->next = entry->next;
            }
            return 1;
        }
        entry = entry->next;
    }
    
    return 0;
}

int dict_delete_by_symbol(JSDict* dict, void* symbol)
{
    JSProperty* entry = dict->buckets[0];

    while (entry)
    {
        if (entry->symbol == symbol)
        {
            if (entry == dict->buckets[0])
            {
                dict->buckets[0] = entry->next;
            }
            else
            {
                JSProperty* prev = dict->buckets[0]->next;
                while (prev->next != entry)
                {
                    prev = prev->next;
                }
                prev->next = entry->next;
            }
            return 1;
        }
        entry = entry->next;
    }

    return 0;
}

