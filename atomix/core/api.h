#ifndef API_H
#define API_H

#include "scope.h"

#define MOD_SECTION ".mod_init"

#ifdef _WIN64

// #pragma section(MOD_SECTION"$a")
// #pragma section(MOD_SECTION"$u")
// #pragma section(MOD_SECTION"$z")

#define MODULE_INIT(fn) \
    __attribute__((used, section(MOD_SECTION"$u"))) static void (*_##fn)(Scope*) = fn

#else

#define MODULE_INIT(fn) \
    __attribute__((used, section(MOD_SECTION))) static void (*_##fn)(Scope*) = fn

#endif

typedef void (*module_init)(Scope*);

void bind_modules(Scope* scope);

char* init_string(char* str);

#endif //API_H
