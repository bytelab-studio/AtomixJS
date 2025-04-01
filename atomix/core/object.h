#ifndef OBJECT_H
#define OBJECT_H

#include "dict.h"

typedef struct JSObject
{
    struct JSObject* prototype;
    JSDict* properties;
} JSObject;

JSObject* object_create_object(JSObject* prototype);

void object_set_property(JSObject* obj, char* key, JSValue value);

JSValue object_get_property(JSObject* obj, char* key);

void object_free(JSObject* obj);

JSObject* object_get_object_prototype();

#endif //OBJECT_H
