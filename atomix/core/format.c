#include "format.impl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gc.h"
#include "panic.h"

#include "api.impl.h"
#include "value.impl.h"

char* string_table_load_str(StringTable* table, uint32_t idx)
{
    if (idx >= table->count)
    {
        PANIC("StringTable idx is out of bounds");
    }

    uint32_t offset = table->offsets[idx];
    uint32_t length = idx == table->count - 1
        ? table->length - (offset + 2 * sizeof(uint32_t) + table->count * sizeof(uint32_t))
        : table->offsets[idx + 1] - offset;
    char* copy = gc_malloc(length + 1);
    memcpy(copy, table->strings + offset, length);
    copy[length] = '\0';
    return copy;
}

JSValue string_table_load_str_value(StringTable* table, uint32_t idx)
{
    if (idx >= table->count)
    {
        PANIC("StringTable idx is out of bounds");
    }

    uint32_t offset = table->offsets[idx];
    uint32_t length = idx == table->count - 1
        ? table->length - (offset + 2 * sizeof(uint32_t) + table->count * sizeof(uint32_t))
        : table->offsets[idx + 1] - offset;

    char* copy = gc_malloc(length + 1);
    memcpy(copy, table->strings + offset, length);
    copy[length] = '\0';

    JSString* string = gc_malloc(sizeof(JSString));
    string->buff = copy;
    string->length = length;

    return JS_VALUE_STRING(string);
}

JSModule* bundle_get_module(JSBundle* bundle, uint64_t hash)
{
    if (bundle) {
        uint16_t low = 0;
        uint16_t high = bundle->moduleCount;
        while(low <= high)
        {
            uint16_t mid = low + (high - low) / 2;

            JSModule* module = &bundle->modules[mid];
            if (module->header.hash == hash)
            {
                return module;
            }

            if (module->header.hash < hash) 
            {
                low = mid + 1;
            } else
            {
                high = mid - 1;
            }
        }
    }
    
    NativeModuleList* entry = native_modules;
    while(entry != NULL)
    {
        if (entry->module->header.hash == hash)
        {
            return entry->module;
        }
        entry = entry->next;
    }

    PANIC("Could not find module in bundle");
}