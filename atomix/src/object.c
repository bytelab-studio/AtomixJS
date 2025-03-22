#include "object.h"

#include <stdlib.h>

#define OBJECT_BUCKET_SIZE 16

JSObject* object_create_object(JSObject* prototype)
{
    JSObject* obj = malloc(sizeof(JSObject));
    obj->prototype = prototype;
    obj->properties = dict_create_dict(OBJECT_BUCKET_SIZE);
    return obj;
}

void object_set_property(JSObject* obj, char* key, JSValue value)
{
    dict_set(obj->properties, key, value, 0);
}

JSValue object_get_property(JSObject* obj, char* key)
{
    JSValue* value = dict_get(obj->properties, key);
    if (value)
    {
        return *value;
    }
    if (obj->prototype && obj->prototype != obj)
    {
        return object_get_property(obj->prototype, key);
    }
    return JS_VALUE_UNDEFINED;
}

void object_free(JSObject* obj)
{
    dict_free(obj->properties);
    free(obj);
}

JSObject* object_prototype = NULL;

JSObject* object_get_object_prototype()
{
    // TODO assign constructor etc.
    if (!object_prototype)
    {
        object_prototype = object_create_object(NULL);
        object_prototype->prototype = object_prototype;
    }

    return object_prototype;
}
