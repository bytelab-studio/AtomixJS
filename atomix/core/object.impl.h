#ifndef OBJECT_IMPL_H
#define OBJECT_IMPL_H

#include "object.h"

#include "dict.h"

struct JSObject
{
    struct JSObject* prototype;
    JSDict* properties;
};

#endif //OBJECT_IMPL_H