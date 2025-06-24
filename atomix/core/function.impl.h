#ifndef FUNCTION_IMPL_H
#define FUNCTION_IMPL_H

#include "function.h"

#include "object.h"

struct JSFunction
{
    JSObject* base;
    JSNativeFunction native_function;
    Scope* scope;
    JSModule* module;

    struct
    {
        size_t instruction_start;
        size_t instruction_end;
    } meta;
    int is_native;
};

#endif //FUNCTION_IMPL_H