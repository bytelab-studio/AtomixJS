#ifndef FORMAT_H
#define FORMAT_H

#include <stdint.h>

typedef struct JSBundle JSBundle;
typedef struct StringTable StringTable;
typedef struct DataSection DataSection;
typedef struct JSModule JSModule;

char* string_table_load_str(StringTable* table, uint32_t idx);

JSModule* bundle_get_module(JSBundle* bundle, uint64_t hash);

#endif //FORMAT_H