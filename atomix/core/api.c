#include "api.h"

#include <stdio.h>
#include <string.h>

#include "allocator.h"

#ifdef _WIN64

__attribute__((section(MOD_SECTION"$a")))
void* __start_mod_init = NULL;

__attribute__((section(MOD_SECTION"$z")))
void* __stop_mod_init = NULL;

#else

extern const module_init __start_mod_init;
extern const module_init __stop_mod_init;

#endif

void bind_modules(Scope* scope)
{
    module_init* start_list = (void*)&__start_mod_init;
    module_init* end_list = (void*)&__stop_mod_init;

    for (module_init* module_init = start_list; module_init < end_list; module_init++)
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
