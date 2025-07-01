#ifndef FORMAT_H
#define FORMAT_H

#include <stdint.h>

#include "value.h"

typedef struct JSBundle JSBundle;
typedef struct StringTable StringTable;
typedef struct DataSection DataSection;
typedef struct JSModule JSModule;

/**
 * Loads a copy of the entry `idx` in the $STABLE
 */
char* string_table_load_str(StringTable* table, uint32_t idx);

/**
 * Loads a copy of entry `idx` in the $STABLE and wraps it in a `JSString` 
 */
JSValue string_table_load_str_value(StringTable* table, uint32_t idx);

JSModule* bundle_get_module(JSBundle* bundle, uint64_t hash);

#endif //FORMAT_H