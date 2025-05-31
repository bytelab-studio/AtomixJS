#ifndef FUNCTION_H
#define FUNCTION_H

#include "module.h"
#include "object.h"
#include "scope.h"
#include "vm.h"

typedef JSValue (*JSNativeFunction)(VM* vm, JSValue this, JSValue* args, size_t argc);

typedef struct
{
    JSObject* base;
    JSNativeFunction native_function;
    Scope* scope;
    JSModule module;

    struct
    {
        size_t instruction_start;
        size_t instruction_end;
    } meta;
    int is_native;
} JSFunction;

JSFunction* function_create_native_function(JSNativeFunction function_ptr);

JSFunction* function_create_function(Scope* parentScope, JSModule module, size_t instruction_start, size_t instruction_end);

#endif //FUNCTION_H
