#ifndef API_H
#define API_H

#include "function.h"
#include "scope.h"
#include "vm.h"

typedef void (*module_init)(Scope*);

JSValue api_call_function(VM* vm, JSFunction* function, JSValue this, JSValue* args, size_t argc);

void bind_modules(Scope* scope);

char* init_string(char* str);

#endif //API_H
