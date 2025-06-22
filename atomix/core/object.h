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

void object_set_property_with_symbol(JSObject* obj, void* symbol, JSValue value);

JSValue object_get_property(JSObject* obj, char* key);

JSValue object_get_property_by_symbol(JSObject* obj, void* symbol);

JSObject* object_get_object_prototype();

JSObject* object_get_array_prototype();

JSObject* object_get_function_prototype();

JSObject* object_get_symbol_prototype();

#endif //OBJECT_H
