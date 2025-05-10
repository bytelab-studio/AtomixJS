#include "api.h"

#include <stdio.h>
#include <string.h>

#include "allocator.h"

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
    char* copy = js_malloc(len + 1);
    strcpy(copy, str);
    copy[len] = '\0';
    return copy;
}
