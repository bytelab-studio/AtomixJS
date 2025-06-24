#ifndef FORMAT_IMPL_H
#define FORMAT_IMPL_H

#include "format.h"

#include "object.h"
#include "scope.h"

#define MODULE_MAGIC0 0x2E
#define MODULE_MAGIC1 0x41
#define MODULE_MAGIC2 0x78
#define MODULE_MAGIC3 0x4D

#define MODULE_VERSION 2

#define BUNDLE_MAGIC0 0x2E
#define BUNDLE_MAGIC1 0x41
#define BUNDLE_MAGIC2 0x78
#define BUNDLE_MAGIC3 0x42

#define BUNDLE_VERSION 1

struct JSModule;

struct JSBundle {
    char magic[4];
    uint16_t version;
    uint64_t entryPoint;
    uint16_t moduleCount;
    struct JSModule* modules;
};

struct StringTable
{
    uint32_t length;
    uint32_t count;
    uint32_t* offsets;
    char* strings;
};

struct DataSection
{
    uint32_t length;
    uint32_t count;
    void** instructions;
};

struct JSModule
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
};

#endif //FORMAT_IMPL_H