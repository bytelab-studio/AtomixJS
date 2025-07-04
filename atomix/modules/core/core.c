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
        JSValue value = value_to_string(vm, args[i]);
        if (value.type != JS_STRING)
        {
            PANIC("Cannot print value");
        }
        printf("%s\n", ((JSString*)value.value.as_pointer)->buff);
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

    if (args[0].type != JS_NUMBER || args[1].type != JS_NUMBER)
    {
        // TODO throw exception
        return JS_VALUE_UNDEFINED;
    }
    uint64_t hash = ((uint64_t)args[0].value.as_number) << 32 | ((uint32_t)args[1].value.as_number);
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

/**
 * [ECMAScript® 2026 Language Specification §20.1.3.6](https://tc39.es/ecma262/#sec-object.prototype.tostring)
 */
JSValue to_string(VM* vm, JSValue this, JSValue* args, size_t argc)
{
    if (this.type == JS_UNDEFINED)
    {
        return init_string_value("[object Undefined]");
    }
    if (this.type == JS_NULL)
    {
        return init_string_value("[object Null]");
    }

    JSObject* obj = value_to_object(this).value.as_pointer;
    JSValue tag = object_get_property_by_symbol(vm, obj, symbol_to_string_tag(vm).value.as_pointer);
    if (!value_is_primitive(tag.type))
    {
        PANIC("Symbol.toStringTag returns a non-primtive value");
    }
    tag = value_to_string(vm, tag);
    tag = value_concat_string(init_string_value("[object "), tag);
    tag = value_concat_string(tag, init_string_value("]"));
    
    return tag;
}

JSValue array(VM* vm, JSValue this, JSValue* args, size_t argc)
{
    JSObject* arr = object_create_object(object_get_array_prototype());

    if (argc == 1)
    {
        JSValue length = args[0];

        if (length.type == JS_NUMBER)
        {
            int size = (int)length.value.as_number;
            object_set_property(vm, arr, init_string("length"), length);
            JSValue value;
            for (int i = 0; i < size; i++)
            {
                value = JS_VALUE_NUMBER(i);
                JSValue key = value_to_string(vm, value);
                object_set_property(vm, arr, ((JSString*)key.value.as_pointer)->buff, JS_VALUE_UNDEFINED);
            }
            return JS_VALUE_OBJECT(arr);
        }
    }

    object_set_property(vm, arr, init_string("length"), JS_VALUE_NUMBER(argc));
    JSValue value;
    for (int i = 0; i < argc; i++)
    {
        value = JS_VALUE_NUMBER(i);
        JSValue key = value_to_string(vm, value);
        object_set_property(vm, arr, ((JSString*)key.value.as_pointer)->buff, args[i]);
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
            description = value_to_string(vm, args[0]);
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

    JSFunction* _to_string = function_create_native_function(to_string);
    object_set_property(vm, _object->base->prototype, init_string("toString"), JS_VALUE_FUNCTION(_to_string));

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
