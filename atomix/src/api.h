#ifndef API_H
#define API_H

#include "scope.h"

#define MOD_SECTION "mod_init"

#define MODULE_INIT(fn) \
    __attribute__((used, section(MOD_SECTION))) static void (*_##fn)(Scope*) = fn

typedef void (*module_init)(Scope*);

void bind_modules(Scope* scope);

#endif //API_H
