#ifndef OBJECT_H
#define OBJECT_H

#include "value.h"

typedef struct JSObject JSObject;

JSObject* object_create_object(JSObject* prototype);

void object_set_property(JSObject* obj, char* key, JSValue value);

JSValue object_get_property(JSObject* obj, char* key);

JSObject* object_get_object_prototype();

JSObject* object_get_array_prototype();

JSObject* object_get_function_prototype();

#endif //OBJECT_H
