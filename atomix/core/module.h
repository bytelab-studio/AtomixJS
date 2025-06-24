#ifndef MODULE_H
#define MODULE_H

#include "dict.h"
#include "object.h"
#include "scope.h"

struct JSModule;

typedef struct {
    char magic[4];
    uint16_t version;
    uint64_t entryPoint;
    uint16_t moduleCount;
    struct JSModule* modules;
} JSBundle;

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

typedef struct JSModule
{
    JSBundle* bundle;

    struct
    {
        char magic[4];
        uint16_t version;
        uint64_t hash;
        uint32_t string_table;
        uint32_t data_section;
    } header;

    StringTable string_table;
    DataSection data_section;
    int initialized;
    JSObject* exports;
    Scope* scope;
} JSModule;

#endif //MODULE_H