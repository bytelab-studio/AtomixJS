#include "api.impl.h"

#include <string.h>
#include <gc.h>

#include "panic.h"
#include "execution.h"

#include "value.impl.h"
#include "function.impl.h"
#include "vm.impl.h"
#include "format.impl.h"

JSValue api_call_function(VM* vm, JSFunction* function, JSValue this, JSValue* args, size_t argc) {
    if (function->is_native) {
        if (!function->native_function) {
            PANIC("Function holds not a valid pointer");            
        }

        return function->native_function(vm, this, args, argc);
    }

    for (size_t i = 0; i < argc; i++) {
        vm->stats.stack[vm->stats.stack_counter++] = args[argc - i - 1];
    }
    
    char* key = init_string("this");
    vm->stats.stack[vm->stats.stack_counter++] = this;
    scope_declare(function->scope, key, this);
    JSValue return_value = vm_exec_function(vm, function);
    vm->stats.stack_counter -= argc;
    return return_value;
}

extern const module_init __MOD_LOADER__[];
extern const size_t __MOD_LOADER_SIZE__;

void bind_modules(Scope* scope) {
    for (size_t i = 0; i < __MOD_LOADER_SIZE__; i++) {
        (__MOD_LOADER__[i])(scope);
    }
}

char* init_string(char* str)
{
    size_t len = strlen(str);
    char* copy = GC_malloc_atomic(len + 1);
    strcpy(copy, str);
    copy[len] = '\0';
    return copy;
}

NativeModuleList* native_modules = NULL;

void register_native_module(uint64_t hash, JSObject* exports)
{
    NativeModuleList* entry = GC_malloc(sizeof(NativeModuleList));
    JSModule* module = GC_malloc(sizeof(JSModule));
    memset(module, 0, sizeof(JSModule));
    module->header.hash = hash;
    module->initialized = 1;
    module->exports = exports;
    
    entry->module = module;
    entry->next = native_modules;

    if (!native_modules)
    {
        native_modules = entry;
        return;
    }

    native_modules = entry;
}
