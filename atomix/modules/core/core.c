#include <stdio.h>
#include <wchar.h>

#include "api.h"
#include "function.h"

JSValue print(VM* vm, JSValue* args, size_t argc)
{
    if (argc == 0)
    {
        printf("\n");
        return JS_VALUE_UNDEFINED;
    }
    for (size_t i = 0; i < argc; i++)
    {
        JSValue value = args[i];
        switch (value.type)
        {
        case JS_INTEGER:
            printf("%i\n", value.value.as_int);
            break;
        case JS_DOUBLE:
            if (value.value.as_double == JS_POS_INFINITY)
            {
                printf("Infinity\n");
                break;
            }
            if (value.value.as_double == JS_NEG_INFINITY)
            {
                printf("-Infinity\n");
                break;
            }
            if (value_is_NaN(&value))
            {
                printf("NaN\n");
                break;
            }

            printf("%f\n", value.value.as_double);
            break;
        case JS_STRING:
            printf("%s\n", (char*)value.value.as_pointer);
            break;
        case JS_OBJECT:
            printf("[Object]\n");
            break;
        case JS_FUNC:
            printf("[Function]\n");
            break;
        case JS_UNDEFINED:
            printf("undefined\n");
            break;
        case JS_NULL:
            printf("null\n");
            break;
        case JS_BOOLEAN:
            printf("%s\n", value.value.as_int ? "true" : "false");
            break;
        }
    }

    return JS_VALUE_UNDEFINED;
}

JSValue module_get_export_obj(VM* vm, JSValue* args, size_t argc)
{
    return JS_VALUE_OBJECT(vm->module.exports);
}

JSValue array(VM* vm, JSValue* args, size_t argc)
{
    JSObject* arr = object_create_object(object_get_array_prototype());

    if (argc == 1)
    {
        JSValue length = args[0];
        if (length.type == JS_DOUBLE)
        {
            // TODO throw exception
            return JS_VALUE_UNDEFINED;
        }

        if (length.type == JS_INTEGER)
        {
            int size = length.value.as_int;
            object_set_property(arr, init_string("length"), length);
            JSValue value;
            for (int i = 0; i < size; i++)
            {
                value = JS_VALUE_INT(i);
                char* key = value_to_string(&value);
                object_set_property(arr, key, JS_VALUE_UNDEFINED);
            }
            return JS_VALUE_OBJECT(arr);
        }
    }

    object_set_property(arr, init_string("length"), JS_VALUE_INT(argc));
    JSValue value;
    for (int i = 0; i < argc; i++)
    {
        value = JS_VALUE_INT(i);
        char* key = value_to_string(&value);
        object_set_property(arr, key, args[i]);
    }
    return JS_VALUE_OBJECT(arr);
}

JSValue is_array(VM* vm, JSValue* args, size_t argc)
{
    if (argc < 1)
    {
        return JS_VALUE_BOOL(0);
    }

    JSValue value = args[0];
    return JS_VALUE_BOOL(value_is_array(&value));
}

void core_init(Scope* scope)
{
    JSFunction* _print = function_create_native_function(print);
    scope_declare(scope, init_string("print"), JS_VALUE_FUNCTION(_print));

    JSObject* _module = object_create_object(object_get_object_prototype());

    JSFunction* _module_get_export_obj = function_create_native_function(module_get_export_obj);
    object_set_property(_module, init_string("getExportObj"), JS_VALUE_FUNCTION(_module_get_export_obj));

    scope_declare(scope, init_string("module"), JS_VALUE_OBJECT(_module));

    JSFunction* _array = function_create_native_function(array);
    _array->base->prototype = object_get_array_prototype();

    JSFunction* _is_array = function_create_native_function(is_array);
    object_set_property(_array->base, init_string("isArray"), JS_VALUE_FUNCTION(_is_array));

    scope_declare(scope, init_string("Array"), JS_VALUE_FUNCTION(_array));
}
