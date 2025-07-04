#include "object.impl.h"

#include <string.h>

#include "gc.h"
#include "api.h"
#include "panic.h"

#include "value.impl.h"

#define OBJECT_BUCKET_SIZE 16

JSObject* object_create_object(JSObject* prototype)
{
    JSObject* obj = gc_malloc(sizeof(JSObject));
    obj->prototype = prototype;
    obj->properties = dict_create_dict(OBJECT_BUCKET_SIZE);
    return obj;
}

void object_set_property(VM* vm, JSObject* obj, char* key, JSValue value)
{
    JSValue* prop = dict_get(obj->properties, key);
    if (prop)
    {
        if (prop->type == JS_GS_BOX)
        {
            JSGSBox* box = prop->value.as_pointer;
            if (!box->setter)
            {
                // TODO throw exception
                PANIC("Property contains no setter");
            }
            api_call_function(vm, box->setter, JS_VALUE_OBJECT(obj), &value, 1);
            return;
        }
        *prop = value;
        return;
    }

    dict_add(obj->properties, key, value);
}

void object_set_property_with_symbol(VM* vm, JSObject* obj, void* symbol, JSValue value)
{
    JSValue* prop = dict_get_by_symbol(obj->properties, symbol);
    if (prop)
    {
        if (prop->type == JS_GS_BOX)
        {
            JSGSBox* box = prop->value.as_pointer;
            if (!box->setter)
            {
                // TODO throw exception
                PANIC("Property contains no setter");
            }
            api_call_function(vm, box->setter, JS_VALUE_OBJECT(obj), &value, 1);
            return;
        }
        *prop = value;
        return;
    }

    dict_add_with_symbol(obj->properties, symbol, value);
}

JSValue object_get_property(VM* vm, JSObject* obj, char* key)
{
    JSValue out;
    if (!object_try_get_property(vm, obj, key, &out))
    {
        return JS_VALUE_UNDEFINED;
    }

    return out;
}

JSValue object_get_property_by_symbol(VM* vm, JSObject* obj, void* symbol)
{
    JSValue out;
    if (!object_try_get_property_by_symbol(vm, obj, symbol, &out))
    {
        return JS_VALUE_UNDEFINED;
    }

    return out;
}

int object_try_get_property(VM* vm, JSObject* obj, char* key, JSValue* out)
{
    JSValue* value = dict_get(obj->properties, key);
    
    if (value)
    {
        if (value->type == JS_GS_BOX)
        {
            JSGSBox* box = value->value.as_pointer;
            if (!box->getter)
            {
                // TODO throw exception
                PANIC("Property contains no getter");
            }

            *out = api_call_function(vm, box->getter, JS_VALUE_OBJECT(obj), NULL, 0);
            return 1;
        }

        *out = *value;
        return 1;
    }
    if (obj->prototype && obj->prototype != obj)
    {
        return object_try_get_property(vm, obj->prototype, key, out);
    }

    return 0;
}

int object_try_get_property_by_symbol(VM* vm, JSObject* obj, void* symbol, JSValue* out)
{
    JSValue* value = dict_get_by_symbol(obj->properties, symbol);

    if (value)
    {
        if (value->type == JS_GS_BOX)
        {
            JSGSBox* box = value->value.as_pointer;
            if (!box->getter)
            {
                // TODO throw exception
                PANIC("Property contains no getter");
            }

            *out = api_call_function(vm, box->getter, JS_VALUE_OBJECT(obj), NULL, 0);
            return 1;
        }

        *out = *value;
        return 1;
    }
    if (obj->prototype && obj->prototype != obj)
    {
        return object_try_get_property_by_symbol(vm, obj->prototype, symbol, out);
    }

    return 0;
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