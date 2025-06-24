#ifndef DICT_H
#define DICT_H

#include <stddef.h>

#include "value.h"

typedef struct JSDict JSDict;

JSDict* dict_create_dict(size_t bucket_count);

JSValue* dict_get(JSDict* dict, char* key);

JSValue* dict_get_by_symbol(JSDict* dict, void* symbol);

void dict_add(JSDict* dict, char* key, JSValue value);

void dict_add_with_symbol(JSDict* dict, void* symbol, JSValue value);

int dict_delete(JSDict* dict, char* key);

int dict_delete_by_symbol(JSDict* dict, void* symbol);

#endif //DICT_H
