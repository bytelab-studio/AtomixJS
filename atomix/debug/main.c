#include <stdio.h>

#include "allocator.h"
#include "loader.h"
#include "format.h"
#include "execution.h"

// #define PRINT_MEMORY_USAGE

static JSModule _module;
static JSBundle _bundle;

static JSModule* module = &_module;
static JSBundle* bundle = &_bundle;

int main(int argc, const char** argv)
{
    if (argc < 2)
    {
        printf("Usage: ./atomix <filename>\n");
        return 1;
    }
    const char* bin_file = argv[1];

    LoadResult result = unknown_load_from_file(bin_file, module, bundle);
    
    if (result == LOAD_BUNDLE) {
        if (!bundle->entryPoint)
        {
            return 0;
        }

        module = bundle_get_module(bundle, bundle->entryPoint);
    }

    VM vm = vm_init(module);
    vm_exec_module(&vm, module);    

#ifdef PRINT_MEMORY_USAGE
    printf("Memory usage:\nAllocated: %zu\nFreed: %zu\n", allocated_memory, freed_memory);
#endif

    return 0;
}
