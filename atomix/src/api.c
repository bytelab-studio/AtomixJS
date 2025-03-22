//
// Created by kosch on 14.03.2025.
//

#include "api.h"

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
