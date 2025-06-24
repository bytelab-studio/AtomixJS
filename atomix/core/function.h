#ifndef FUNCTION_H
#define FUNCTION_H

#include <stddef.h>

#include "format.h"
#include "scope.h"
#include "value.h"
#include "vm.h"

typedef JSValue (*JSNativeFunction)(VM* vm, JSValue this, JSValue* args, size_t argc);

typedef struct JSFunction JSFunction;

JSFunction* function_create_native_function(JSNativeFunction function_ptr);

JSFunction* function_create_function(Scope* parentScope, JSModule* module, size_t instruction_start, size_t instruction_end);

#endif //FUNCTION_H
