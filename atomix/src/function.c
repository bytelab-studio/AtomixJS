#include "function.h"

#include "allocator.h"

JSFunction* function_create_native_function(JSNativeFunction function_ptr)
{
    JSFunction* function = js_malloc(sizeof(JSFunction));
    function->is_native = 1;
    function->native_function = function_ptr;
    function->base = object_create_object(object_get_object_prototype());

    return function;
}

JSFunction* function_create_function(
    Scope* parentScope,
    JSModule module,
    size_t instruction_start,
    size_t instruction_end)
{
    // TODO add name to constructor
    JSFunction* function = js_malloc(sizeof(JSFunction));
    function->is_native = 0;
    function->meta.instruction_start = instruction_start;
    function->meta.instruction_end = instruction_end;
    function->module = module;

    Scope* scope = scope_create_scope(parentScope);
    function->scope = scope;
    function->base = object_create_object(object_get_object_prototype());

    return function;
}
