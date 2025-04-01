#ifndef DICT_H
#define DICT_H

#include <stddef.h>

#include "value.h"


typedef struct JSProperty
{
    char* key;
    JSValue value;
    struct JSProperty* next;
} JSProperty;

typedef struct
{
    JSProperty** buckets;
    size_t bucket_count;
} JSDict;

JSDict* dict_create_dict(size_t bucket_count);

int dict_set(JSDict* dict, char* key, JSValue value, int update_only);

JSValue* dict_get(JSDict* dict, char* key);

int dict_delete(JSDict* dict, char* key);

void dict_free(JSDict* dict);



#endif //DICT_H
