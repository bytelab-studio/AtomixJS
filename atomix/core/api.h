#ifndef API_H
#define API_H

#include "function.h"
#include "scope.h"
#include "vm.h"

typedef void (*module_init)(VM* vm, Scope*);

typedef struct NativeModuleList {
    struct NativeModuleList* next;
    JSModule* module;
} NativeModuleList;

extern NativeModuleList* native_modules;

JSValue api_call_function(VM* vm, JSFunction* function, JSValue this, JSValue* args, size_t argc);

void bind_modules(VM* vm, Scope* scope);

char* init_string(char* str);

void register_native_module(uint64_t hash, JSObject* object);

#endif //API_H
