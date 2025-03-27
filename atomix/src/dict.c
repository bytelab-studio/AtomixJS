#include "dict.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "allocator.h"
#include "panic.h"

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
    JSDict* dict = js_malloc(sizeof(JSDict));
    if (!dict)
    {
        PANIC("Could not allocate memory");
    }
    dict->buckets = js_calloc(bucket_count, sizeof(JSProperty*));
    dict->bucket_count = bucket_count;
    return dict;
}

int dict_set(JSDict* dict, char* key, JSValue value, int update_only)
{
    size_t index = hash_string(key, dict->bucket_count);
    JSProperty* entry = dict->buckets[index];

    while (entry)
    {
        if (strcmp(key, entry->key) == 0)
        {
            entry->value = value;
            return 1;
        }
        entry = entry->next;
    }

    if (update_only)
    {
        return 0;
    }

    JSProperty* new_entry = js_malloc(sizeof(JSProperty));
    if (!new_entry)
    {
        PANIC("Could not allocate memory");
    }
    new_entry->key = key;
    new_entry->value = value;
    new_entry->next = dict->buckets[index];
    dict->buckets[index] = new_entry;
    return 1;
}

int dict_update(JSDict* dict, char* key, JSValue value)
{
    size_t index = hash_string(key, dict->bucket_count);
    JSProperty* entry = dict->buckets[index];

    while (entry)
    {
        if (strcmp(key, entry->key) == 0)
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
        if (strcmp(key, entry->key) == 0)
        {
            return &entry->value;
        }
        entry = entry->next;
    }

    return NULL;
}

int dict_delete(JSDict* dict, char* key)
{
    size_t index = hash_string(key, dict->bucket_count);
    JSProperty* entry = dict->buckets[index];

    while (entry)
    {
        if (strcmp(key, entry->key) == 0)
        {
            if (entry == dict->buckets[index])
            {
                dict->buckets[index] = entry->next;
            }else
            {
                JSProperty* prev = dict->buckets[index]->next;
                while (prev->next != entry)
                {
                    prev = prev->next;
                }
                prev->next = entry->next;
            }
            js_free(entry->key);
            js_free(entry);
            return 1;
        }
    }
    return 0;
}

void dict_free(JSDict* dict)
{
    for (size_t i = 0; i < dict->bucket_count; i++)
    {
        JSProperty* entry = dict->buckets[i];
        while (entry)
        {
            JSProperty* next = entry->next;
            js_free(entry->key);
            js_free(entry);
            entry = next;
        }
    }
    if (dict->buckets)
    {
        js_free(dict->buckets);
    }
    js_free(dict);
}
