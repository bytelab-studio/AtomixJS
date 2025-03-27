#include "api.h"

#include <string.h>

#include "allocator.h"

extern module_init __start_mod_init;
extern module_init __stop_mod_init;


void bind_modules(Scope* scope)
{
    for (module_init* module_init = &__start_mod_init; module_init < &__stop_mod_init; module_init++)
    {
        if (*module_init)
        {
            (*module_init)(scope);
        }
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
