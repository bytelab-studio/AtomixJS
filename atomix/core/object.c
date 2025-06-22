#include "object.h"

#include <string.h>
#include <gc.h>

#define OBJECT_BUCKET_SIZE 16

JSObject* object_create_object(JSObject* prototype)
{
    JSObject* obj = GC_malloc(sizeof(JSObject));
    obj->prototype = prototype;
    obj->properties = dict_create_dict(OBJECT_BUCKET_SIZE);
    return obj;
}

void object_set_property(JSObject* obj, char* key, JSValue value)
{
    dict_set(obj->properties, key, value, 0);
}

void object_set_property_with_symbol(JSObject* obj, void* symbol, JSValue value)
{
    dict_set_with_symbol(obj->properties, symbol, value, 0);
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

JSValue object_get_property_by_symbol(JSObject* obj, void* symbol)
{
    JSValue* value = dict_get_by_symbol(obj->properties, symbol);

    if (value)
    {
        return *value;
    }
    if (obj->prototype && obj->prototype != obj)
    {
        return object_get_property_by_symbol(obj->prototype, symbol);
    }

    return JS_VALUE_UNDEFINED;
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
    // TODO assign constructor etc.
    if (!function_prototype)
    {
        function_prototype = object_create_object(object_get_object_prototype());
    }

    return function_prototype;
}

JSObject* symbol_prototype = NULL;

JSObject* object_get_symbol_prototype()
{
    if (!symbol_prototype)
    {
        symbol_prototype = object_create_object(object_get_object_prototype());
    }

    return symbol_prototype;
}