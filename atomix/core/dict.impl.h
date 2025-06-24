#ifndef DICT_IMPL_H
#define DICT_IMPL_H

#include "dict.h"

#include "value.impl.h"

typedef struct JSProperty
{
    char* key;
    JSValue value;
    struct JSProperty* next;
} JSProperty;

struct JSDict
{
    JSProperty** buckets;
    size_t bucket_count;
};

#endif //DICT_IMPL_H