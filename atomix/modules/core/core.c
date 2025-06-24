#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "AtomixJS.h"

#include "vm.impl.h"
#include "value.impl.h"
#include "format.impl.h"
#include "function.impl.h"
#include "object.impl.h"

JSValue print(VM* vm, JSValue this, JSValue* args, size_t argc)
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
        case JS_SYMBOL:
            printf("[Symbol]\n");
            break;
        }
    }

    return JS_VALUE_UNDEFINED;
}

JSValue module_get_export_obj(VM* vm, JSValue this, JSValue* args, size_t argc)
{
    return JS_VALUE_OBJECT(vm->module->exports);
}

JSValue module_import_module(VM* vm, JSValue this, JSValue* args, size_t argc)
{
    if (argc < 2)
    {
        // TODO throw exception
        return JS_VALUE_UNDEFINED;
    }

    if (args[0].type != JS_INTEGER || args[1].type != JS_INTEGER)
    {
        // TODO throw exception
        return JS_VALUE_UNDEFINED;
    }
    uint64_t hash = ((uint64_t)args[0].value.as_int) << 32 | ((uint32_t)args[1].value.as_int);
    JSModule* module = bundle_get_module(vm->module->bundle, hash);
    if (!module->initialized)
    {
        module->initialized = 1;
        vm_exec_module(vm, module);
    }

    return JS_VALUE_OBJECT(module->exports);
}

JSValue object(VM* vm, JSValue this, JSValue* args, size_t argc)
{
    // TODO implement
    return JS_VALUE_UNDEFINED;
}

JSValue instantiate(VM* vm, JSValue this, JSValue* args, size_t argc)
{
    if (argc == 0)
    {
        // TODO throw exception
        return JS_VALUE_UNDEFINED;
    }

    JSValue constructor_wrapped = args[0];
    if (constructor_wrapped.type != JS_FUNC)
    {
        // TODO throw exception
        return JS_VALUE_UNDEFINED;
    }
    JSFunction* constructor = constructor_wrapped.value.as_pointer;

    JSValue prototype_wrapped = object_get_property(vm, constructor->base, "prototype");
    if (prototype_wrapped.type != JS_OBJECT)
    {
        // TODO throw exception
        return JS_VALUE_UNDEFINED;
    }

    JSObject* obj = object_create_object((JSObject*)prototype_wrapped.value.as_pointer);
    JSValue return_value = api_call_function(vm, constructor, JS_VALUE_OBJECT(obj), args + 1, argc - 1);
    if (return_value.type == JS_OBJECT)
    {
        return return_value;
    }

    return JS_VALUE_OBJECT(obj);
}

JSValue create(VM* vm, JSValue this, JSValue* args, size_t argc)
{
    if (argc == 0)
    {
        return JS_VALUE_OBJECT(object_create_object(object_get_object_prototype()));
    }

    if (args[0].type != JS_OBJECT)
    {
        // TODO throw exception
        return JS_VALUE_OBJECT(object_create_object(object_get_object_prototype()));
    }

    return JS_VALUE_OBJECT(object_create_object((JSObject*)args[0].value.as_pointer));
}

JSValue setPrototypeOf(VM* vm, JSValue this, JSValue* args, size_t argc)
{
    if (argc < 2)
    {
        return JS_VALUE_UNDEFINED;
    }

    if ((args[0].type != JS_OBJECT && args[0].type != JS_FUNC) ||
        (args[1].type != JS_OBJECT && args[1].type != JS_FUNC))
    {
        // TODO throw exception
        return JS_VALUE_UNDEFINED;
    }

    JSObject* target = args[0].type == JS_OBJECT
        ? (JSObject*)args[0].value.as_pointer
        : ((JSFunction*)args[0].value.as_pointer)->base;
    JSObject* prototype = args[1].type == JS_OBJECT
        ? (JSObject*)args[1].value.as_pointer
        : ((JSFunction*)args[1].value.as_pointer)->base;

    target->prototype = prototype;
    return JS_VALUE_UNDEFINED;
}

JSValue array(VM* vm, JSValue this, JSValue* args, size_t argc)
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
            object_set_property(vm, arr, init_string("length"), length);
            JSValue value;
            for (int i = 0; i < size; i++)
            {
                value = JS_VALUE_INT(i);
                char* key = value_to_string(&value);
                object_set_property(vm, arr, key, JS_VALUE_UNDEFINED);
            }
            return JS_VALUE_OBJECT(arr);
        }
    }

    object_set_property(vm, arr, init_string("length"), JS_VALUE_INT(argc));
    JSValue value;
    for (int i = 0; i < argc; i++)
    {
        value = JS_VALUE_INT(i);
        char* key = value_to_string(&value);
        object_set_property(vm, arr, key, args[i]);
    }
    return JS_VALUE_OBJECT(arr);
}

JSValue is_array(VM* vm, JSValue this, JSValue* args, size_t argc)
{
    if (argc < 1)
    {
        return JS_VALUE_BOOL(0);
    }

    JSValue value = args[0];
    return JS_VALUE_BOOL(value_is_array(&value));
}

JSValue function(VM* vm, JSValue this, JSValue* args, size_t argc) {
    // TODO throw exception
    return JS_VALUE_UNDEFINED;
}

JSValue call(VM* vm, JSValue this, JSValue* args, size_t argc) {
    if (this.type != JS_FUNC) {
        // TODO throw execption
        return JS_VALUE_UNDEFINED;
    }

    JSFunction* function = this.value.as_pointer;
    if (argc == 0) {
        return api_call_function(vm, function, JS_VALUE_UNDEFINED, args, argc);
    }
    return api_call_function(vm, function, args[0], args + 1, argc - 1);
}

JSValue symbol(VM* vm, JSValue this, JSValue* args, size_t argc)
{
    JSObject* symbol = object_create_object(object_get_symbol_prototype());
    if (argc > 0)
    {
        JSValue description = args[0];

        if (description.type != JS_STRING)
        {
            description = JS_VALUE_STRING(value_to_string(&args[0]));
        }

        object_set_property(vm, symbol, init_string("description"), description);
    }

    return JS_VALUE_SYMBOL(symbol);
}

void core_init(VM* vm, Scope* scope)
{
    // Helper
    JSFunction* _print = function_create_native_function(print);
    scope_declare(scope, init_string("print"), JS_VALUE_FUNCTION(_print));

    // Module
    JSObject* _module = object_create_object(object_get_object_prototype());

    JSFunction* _module_get_export_obj = function_create_native_function(module_get_export_obj);
    object_set_property(vm, _module, init_string("getExportObj"), JS_VALUE_FUNCTION(_module_get_export_obj));

    JSFunction* _module_import_module = function_create_native_function(module_import_module);
    object_set_property(vm, _module, init_string("importModule"), JS_VALUE_FUNCTION(_module_import_module));

    scope_declare(scope, init_string("Module"), JS_VALUE_OBJECT(_module));

    // Object
    JSFunction* _object = function_create_native_function(object);

    JSFunction* _instantiate = function_create_native_function(instantiate);
    object_set_property(vm, _object->base, init_string("instantiate"), JS_VALUE_FUNCTION(_instantiate));

    JSFunction* _create = function_create_native_function(create);
    object_set_property(vm, _object->base, init_string("create"), JS_VALUE_FUNCTION(_create));

    JSFunction* _setPrototypeOf = function_create_native_function(setPrototypeOf);
    object_set_property(vm, _object->base, init_string("setPrototypeOf"), JS_VALUE_FUNCTION(_setPrototypeOf));

    scope_declare(scope, init_string("Object"), JS_VALUE_FUNCTION(_object));

    // Array
    JSFunction* _array = function_create_native_function(array);
    _array->base->prototype = object_get_array_prototype();

    JSFunction* _is_array = function_create_native_function(is_array);
    object_set_property(vm, _array->base, init_string("isArray"), JS_VALUE_FUNCTION(_is_array));

    scope_declare(scope, init_string("Array"), JS_VALUE_FUNCTION(_array));

    // Function
    JSFunction* _function = function_create_native_function(function);
    _function->base->prototype = object_get_function_prototype();

    scope_declare(scope, init_string("Function"), JS_VALUE_FUNCTION(_function));

    JSFunction* _call = function_create_native_function(call);
    object_set_property(vm, _function->base->prototype, init_string("call"), JS_VALUE_FUNCTION(_call));

    // Symbol
    JSFunction* _symbol = function_create_native_function(symbol);
    _symbol->base->prototype = object_get_symbol_prototype();

    object_set_property(vm, _symbol->base, init_string("toPrimitive"), symbol_to_primitive(vm));

    scope_declare(scope, init_string("Symbol"), JS_VALUE_FUNCTION(_symbol));
}
