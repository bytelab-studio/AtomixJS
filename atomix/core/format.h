#ifndef FORMAT_H
#define FORMAT_H

#include "module.h"

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

char* string_table_load_str(StringTable* table, uint32_t idx);

JSModule* bundle_get_module(JSBundle* bundle, uint64_t hash);

#endif //FORMAT_H