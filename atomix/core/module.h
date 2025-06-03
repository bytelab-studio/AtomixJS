#ifndef MODULE_H
#define MODULE_H

#include "dict.h"
#include "object.h"

typedef struct
{
    uint32_t length;
    uint32_t count;
    uint32_t* offsets;
    char* strings;
} StringTable;

typedef struct
{
    uint32_t length;
    uint32_t count;
    void** instructions;
} DataSection;

typedef struct
{
    struct
    {
        char magic[3];
        uint16_t version;
        uint64_t hash;
        uint32_t string_table;
        uint32_t data_section;
    } header;

    StringTable string_table;
    DataSection data_section;
    JSObject* exports;
} JSModule;

char* string_table_load_str(StringTable* table, uint32_t idx);

#endif //MODULE_H
