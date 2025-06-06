#include "bundle.h"

#include <stdlib.h>
#include <stdio.h>
#include "panic.h"

JSModule bundle_get_module(JSBundle bundle, uint64_t hash)
{
    uint16_t low = 0;
    uint16_t high = bundle.moduleCount;
    while(1)
    {
        uint16_t mid = low + (high - low) / 2;

        JSModule module = bundle.modules[mid];
        if (module.header.hash == hash)
        {
            return module;
        }

        if (module.header.hash < hash) 
        {
            low = mid + 1;
        } else
        {
            high = mid - 1;
        }
    }

    PANIC("Could not find module in bundle");
}