#include <string.h>
#include "object.h"
#include "allocator.h"

#define OBJECT_BUCKET_SIZE 16

JSObject* object_create_object(JSObject* prototype)
{
    JSObject* obj = js_malloc(sizeof(JSObject));
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
    if (strcmp(key, "prototype") == 0) 
    {
        return JS_VALUE_OBJECT(obj->prototype);
    }

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
    js_free(obj);
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

JSObject* array_prototype = NULL;

JSObject* object_get_array_prototype()
{
    // TODO assign constructor etc.
    if (!array_prototype)
    {
        array_prototype = object_create_object(object_get_object_prototype());
    }

    return array_prototype;
}

JSObject* function_prototype = NULL;

JSObject* object_get_function_prototype()
{
    if (!function_prototype)
    {
        function_prototype = object_create_object(object_get_object_prototype());
    }

    return function_prototype;
}