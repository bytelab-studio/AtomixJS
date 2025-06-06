#include "format.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "allocator.h"
#include "panic.h"

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
    char* copy = js_malloc(length + 1);
    memcpy(copy, table->strings + offset, length);
    copy[length] = '\0';
    return copy;
}

JSModule* bundle_get_module(JSBundle* bundle, uint64_t hash)
{
    uint16_t low = 0;
    uint16_t high = bundle->moduleCount;
    while(1)
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

    PANIC("Could not find module in bundle");
}